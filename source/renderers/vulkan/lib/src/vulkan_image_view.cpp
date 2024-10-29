// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_image_view.hpp"

#include <utility>  // exchange

#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile::vk {

VulkanImageView::~VulkanImageView() noexcept
{
  _destroy();
}

VulkanImageView::VulkanImageView(VulkanImageView&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

auto VulkanImageView::operator=(VulkanImageView&& other) noexcept -> VulkanImageView&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

void VulkanImageView::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroyImageView(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto create_vulkan_image_view(VkDevice device,
                              VkImage image,
                              const VkFormat format,
                              const VkImageViewType type,
                              const VkImageAspectFlags aspect_mask,
                              const std::uint32_t mip_levels)
    -> std::expected<VulkanImageView, VkResult>
{
  const VkImageViewCreateInfo image_view_info {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .image = image,
    .viewType = type,
    .format = format,
    .components =
        VkComponentMapping {
          .r = VK_COMPONENT_SWIZZLE_IDENTITY,
          .g = VK_COMPONENT_SWIZZLE_IDENTITY,
          .b = VK_COMPONENT_SWIZZLE_IDENTITY,
          .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },
    .subresourceRange =
        VkImageSubresourceRange {
          .aspectMask = aspect_mask,
          .baseMipLevel = 0,
          .levelCount = mip_levels,
          .baseArrayLayer = 0,
          .layerCount = 1,
        },
  };

  VulkanImageView image_view {};
  image_view.device = device;

  const auto result = vkCreateImageView(device, &image_view_info, nullptr, &image_view.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan image view: {}", to_string(result));
    return std::unexpected {result};
  }

  return image_view;
}

}  // namespace tactile::vk
