// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_image.hpp"

#include <cassert>        // assert
#include <unordered_map>  // unordered_map
#include <utility>        // exchange

#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_buffer.hpp"
#include "tactile/vulkan/vulkan_command_pool.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile::vk {
namespace {

// Used to determine access flags for layout transitions.
const std::unordered_map<VkImageLayout, VkAccessFlags> kTransitionAccessMap {
  {VK_IMAGE_LAYOUT_UNDEFINED, 0},
  {VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_TRANSFER_READ_BIT},
  {VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT},
  {VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT},
  {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
   VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT},
};

// Used to determine pipeline stage flags for layout transitions.
const std::unordered_map<VkImageLayout, VkPipelineStageFlags> kTransitionStageMap {
  {VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT},
  {VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT},
  {VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT},
  {VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT},
  {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
};

void _do_change_layout(VkCommandBuffer command_buffer,
                       VkImage image,
                       const VkImageLayout old_layout,
                       const VkImageLayout new_layout,
                       const std::uint32_t base_mip_level,
                       const std::uint32_t level_count)
{
  const auto src_access = kTransitionAccessMap.at(old_layout);
  const auto dst_access = kTransitionAccessMap.at(new_layout);

  const auto src_stage = kTransitionStageMap.at(old_layout);
  const auto dst_stage = kTransitionStageMap.at(new_layout);

  const VkImageMemoryBarrier image_memory_barrier {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = nullptr,
    .srcAccessMask = src_access,
    .dstAccessMask = dst_access,
    .oldLayout = old_layout,
    .newLayout = new_layout,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = image,
    .subresourceRange =
        VkImageSubresourceRange {
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .baseMipLevel = base_mip_level,
          .levelCount = level_count,
          .baseArrayLayer = 0,
          .layerCount = 1,
        },
  };

  vkCmdPipelineBarrier(command_buffer,
                       src_stage,
                       dst_stage,
                       0,
                       0,
                       nullptr,
                       0,
                       nullptr,
                       1,
                       &image_memory_barrier);
}

}  // namespace

VulkanImage::VulkanImage(VulkanImage&& other) noexcept
  : allocator {std::exchange(other.allocator, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)},
    allocation {std::exchange(other.allocation, VK_NULL_HANDLE)},
    params {std::exchange(other.params, VulkanImageParams {})}
{}

VulkanImage::~VulkanImage() noexcept
{
  _destroy();
}

auto VulkanImage::operator=(VulkanImage&& other) noexcept -> VulkanImage&
{
  if (this != &other) {
    _destroy();

    allocator = std::exchange(other.allocator, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
    allocation = std::exchange(other.allocation, VK_NULL_HANDLE);
    params = std::exchange(other.params, VulkanImageParams {});
  }

  return *this;
}

void VulkanImage::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vmaDestroyImage(allocator, handle, allocation);
    handle = VK_NULL_HANDLE;
  }
}

auto VulkanImage::change_layout(VkDevice device,
                                VkQueue queue,
                                VkCommandPool command_pool,
                                const VkImageLayout new_layout) -> VkResult
{
  if (params.layout == new_layout) {
    return VK_SUCCESS;
  }

  const auto work = [this, new_layout](VkCommandBuffer command_buffer) {
    _do_change_layout(command_buffer, handle, params.layout, new_layout, 0, params.mip_levels);
    params.layout = new_layout;
  };

  return record_and_submit_commands(device, queue, command_pool, work);
}

auto VulkanImage::set_data(VkDevice device,
                           VkQueue queue,
                           VkCommandPool command_pool,
                           const void* data,
                           const std::uint64_t data_size) -> VkResult
{
  auto staging_buffer = create_vulkan_staging_buffer(allocator, data_size, 0);
  if (!staging_buffer) {
    return staging_buffer.error();
  }

  auto result = set_buffer_data(*staging_buffer, data, data_size);
  if (result != VK_SUCCESS) {
    return result;
  }

  result = change_layout(device, queue, command_pool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  if (result != VK_SUCCESS) {
    return result;
  }

  result = copy_buffer(device, queue, command_pool, staging_buffer->handle);
  if (result != VK_SUCCESS) {
    return result;
  }

  result =
      change_layout(device, queue, command_pool, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  if (result != VK_SUCCESS) {
    return result;
  }

  return result;
}

auto VulkanImage::copy_buffer(VkDevice device,
                              VkQueue queue,
                              VkCommandPool command_pool,
                              VkBuffer buffer) -> VkResult
{
  auto work = [&](VkCommandBuffer command_buffer) {
    const VkBufferImageCopy region = {
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource =
          VkImageSubresourceLayers {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
          },
      .imageOffset = VkOffset3D {0, 0, 0},
      .imageExtent = VkExtent3D {params.extent.width, params.extent.height, 1},
    };

    vkCmdCopyBufferToImage(command_buffer, buffer, handle, params.layout, 1, &region);
  };

  return record_and_submit_commands(device, queue, command_pool, work);
}

auto VulkanImage::generate_mipmaps(VkDevice device, VkQueue queue, VkCommandPool command_pool)
    -> VkResult
{
  assert(params.mip_levels > 0);

  const auto work = [this](VkCommandBuffer command_buffer) {
    auto mip_width = params.extent.width;
    auto mip_height = params.extent.height;

    for (std::uint32_t mip_level = 1u; mip_level < params.mip_levels; ++mip_level) {
      const std::uint32_t base_mip_level = mip_level - 1u;

      _do_change_layout(command_buffer,
                        handle,
                        params.layout,
                        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                        base_mip_level,
                        1);

      const VkImageBlit image_blit {
        .srcSubresource =
            VkImageSubresourceLayers {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = base_mip_level,
              .baseArrayLayer = 0,
              .layerCount = 1,
            },
        .srcOffsets =
            {
              VkOffset3D {0, 0, 0},
              VkOffset3D {static_cast<std::int32_t>(mip_width),
                          static_cast<std::int32_t>(mip_height),
                          1},
            },
        .dstSubresource =
            VkImageSubresourceLayers {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = mip_level,
              .baseArrayLayer = 0,
              .layerCount = 1,
            },
        .dstOffsets =
            {
              VkOffset3D {0, 0, 0},
              VkOffset3D {static_cast<std::int32_t>(mip_width > 1 ? (mip_width / 2) : 1),
                          static_cast<std::int32_t>(mip_height > 1 ? (mip_height / 2) : 1),
                          1},
            },
      };

      vkCmdBlitImage(command_buffer,
                     handle,
                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                     handle,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     1,
                     &image_blit,
                     VK_FILTER_NEAREST);

      _do_change_layout(command_buffer,
                        handle,
                        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        base_mip_level,
                        1);

      if (mip_width > 1) {
        mip_width /= 2;
      }

      if (mip_height > 1) {
        mip_height /= 2;
      }
    }

    // Transition the last mipmap image to the optimal shader read layout
    _do_change_layout(command_buffer,
                      handle,
                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                      params.mip_levels - 1,
                      1);

    params.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  };

  return record_and_submit_commands(device, queue, command_pool, work);
}

auto create_vulkan_image(VmaAllocator allocator, const VulkanImageParams& params)
    -> std::expected<VulkanImage, VkResult>
{
  const VkImageCreateInfo image_info {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .imageType = params.type,
    .format = params.format,
    .extent = VkExtent3D {params.extent.width, params.extent.height, 1},
    .mipLevels = params.mip_levels,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = params.usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr,
    .initialLayout = params.layout,
  };

  constexpr VmaAllocationCreateInfo allocation_info {
    .flags = 0,
    .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
    .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    .preferredFlags = 0,
    .memoryTypeBits = 0,
    .pool = nullptr,
    .pUserData = nullptr,
    .priority = 0.0f,
  };

  VulkanImage image {};
  image.allocator = allocator;
  image.params = params;

  const auto result = vmaCreateImage(allocator,
                                     &image_info,
                                     &allocation_info,
                                     &image.handle,
                                     &image.allocation,
                                     nullptr);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan image: {}", to_string(result));
    return std::unexpected {result};
  }

  return image;
}

}  // namespace tactile::vk
