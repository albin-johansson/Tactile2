// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_buffer.hpp"

#include <algorithm>  // min
#include <cstring>    // memcpy
#include <utility>    // exchange

#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile {

VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
  : allocator {std::exchange(other.allocator, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)},
    allocation {std::exchange(other.allocation, VK_NULL_HANDLE)}
{}

VulkanBuffer::~VulkanBuffer() noexcept
{
  _destroy();
}

auto VulkanBuffer::operator=(VulkanBuffer&& other) noexcept -> VulkanBuffer&
{
  if (this != &other) {
    _destroy();

    allocator = std::exchange(other.allocator, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
    allocation = std::exchange(other.allocation, VK_NULL_HANDLE);
  }

  return *this;
}

void VulkanBuffer::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vmaDestroyBuffer(allocator, handle, allocation);
    handle = VK_NULL_HANDLE;
  }
}

auto create_vulkan_buffer(VmaAllocator allocator,
                          const VkBufferCreateInfo& buffer_info,
                          const VmaAllocationCreateInfo& allocation_info)
    -> std::expected<VulkanBuffer, VkResult>
{
  VulkanBuffer buffer {};
  buffer.allocator = allocator;

  const auto result = vmaCreateBuffer(allocator,
                                      &buffer_info,
                                      &allocation_info,
                                      &buffer.handle,
                                      &buffer.allocation,
                                      nullptr);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan buffer: {}", to_string(result));
    return std::unexpected {result};
  }

  return buffer;
}

auto create_vulkan_staging_buffer(VmaAllocator allocator,
                                  const std::uint64_t size,
                                  const VkBufferUsageFlags usage_flags)
    -> std::expected<VulkanBuffer, VkResult>
{
  const VkBufferCreateInfo buffer_info {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .size = size,
    .usage = usage_flags | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr,
  };

  constexpr VmaAllocationCreateInfo allocation_info {
    .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
    .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
    .requiredFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    .memoryTypeBits = 0,
    .pool = nullptr,
    .pUserData = nullptr,
    .priority = 0,
  };

  return create_vulkan_buffer(allocator, buffer_info, allocation_info);
}

auto set_buffer_data(VulkanBuffer& buffer, const void* data, const std::uint64_t data_size)
    -> VkResult
{
  void* mapped_data = nullptr;
  const auto result = vmaMapMemory(buffer.allocator, buffer.allocation, &mapped_data);

  if (result != VK_SUCCESS) {
    return result;
  }

  VmaAllocationInfo allocation_info {};
  vmaGetAllocationInfo(buffer.allocator, buffer.allocation, &allocation_info);

  const auto allocation_size = static_cast<std::uint64_t>(allocation_info.size);
  std::memcpy(mapped_data, data, std::min(data_size, allocation_size));

  vmaUnmapMemory(buffer.allocator, buffer.allocation);

  return result;
}

}  // namespace tactile
