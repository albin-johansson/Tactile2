// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_command_buffer.hpp"

#include <utility>  // exchange

#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile {

VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    pool {std::exchange(other.pool, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

auto VulkanCommandBuffer::operator=(VulkanCommandBuffer&& other) noexcept
    -> VulkanCommandBuffer&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    pool = std::exchange(other.pool, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

VulkanCommandBuffer::~VulkanCommandBuffer() noexcept
{
  _destroy();
}

void VulkanCommandBuffer::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkFreeCommandBuffers(device, pool, 1, &handle);
    handle = VK_NULL_HANDLE;
  }
}

auto create_vulkan_command_buffer(VkDevice device, VkCommandPool pool)
    -> std::expected<VulkanCommandBuffer, VkResult>
{
  const VkCommandBufferAllocateInfo command_buffer_allocate_info {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = nullptr,
    .commandPool = pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };

  VulkanCommandBuffer command_buffer {};
  command_buffer.device = device;
  command_buffer.pool = pool;

  const auto result =
      vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan command buffer: {}", to_string(result));
    return std::unexpected {result};
  }

  return command_buffer;
}

}  // namespace tactile
