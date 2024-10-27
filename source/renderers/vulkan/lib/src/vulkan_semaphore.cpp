// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_semaphore.hpp"

#include <utility>  // exchange

#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile {

VulkanSemaphore::VulkanSemaphore(VulkanSemaphore&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanSemaphore::~VulkanSemaphore() noexcept
{
  _destroy();
}

void VulkanSemaphore::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroySemaphore(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto VulkanSemaphore::operator=(VulkanSemaphore&& other) noexcept -> VulkanSemaphore&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

auto create_vulkan_semaphore(VkDevice device) -> std::expected<VulkanSemaphore, VkResult>
{
  constexpr VkSemaphoreCreateInfo create_info {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
  };

  VulkanSemaphore semaphore {};
  semaphore.device = device;

  const auto result = vkCreateSemaphore(device, &create_info, nullptr, &semaphore.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan semaphore: {}", to_string(result));
    return std::unexpected {result};
  }

  return semaphore;
}

}  // namespace tactile
