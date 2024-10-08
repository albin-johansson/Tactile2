// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_command_pool.hpp"

#include <utility>  // exchange

#include "tactile/runtime/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile {

VulkanCommandPool::VulkanCommandPool(VulkanCommandPool&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanCommandPool::~VulkanCommandPool() noexcept
{
  _destroy();
}

auto VulkanCommandPool::operator=(VulkanCommandPool&& other) noexcept -> VulkanCommandPool&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

void VulkanCommandPool::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroyCommandPool(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto create_vulkan_command_pool(VkDevice device,
                                const std::uint32_t queue_family_index,
                                const VkCommandPoolCreateFlags flags)
    -> std::expected<VulkanCommandPool, VkResult>
{
  const VkCommandPoolCreateInfo pool_info {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = flags,
    .queueFamilyIndex = queue_family_index,
  };

  VulkanCommandPool command_pool {};
  command_pool.device = device;

  const auto result = vkCreateCommandPool(device, &pool_info, nullptr, &command_pool.handle);

  if (result != VK_SUCCESS) {
    log(LogLevel::kError, "Could not create Vulkan command pool: {}", to_string(result));
    return std::unexpected {result};
  }

  return command_pool;
}

}  // namespace tactile
