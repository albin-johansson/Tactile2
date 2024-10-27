// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_fence.hpp"

#include <utility>  // exchange

#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile {

VulkanFence::VulkanFence(VulkanFence&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanFence::~VulkanFence() noexcept
{
  _destroy();
}

void VulkanFence::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroyFence(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto VulkanFence::operator=(VulkanFence&& other) noexcept -> VulkanFence&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

auto create_vulkan_fence(VkDevice device, const VkFenceCreateFlags flags)
    -> std::expected<VulkanFence, VkResult>
{
  const VkFenceCreateInfo create_info {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = flags,
  };

  VulkanFence fence {};
  fence.device = device;
  const auto result = vkCreateFence(device, &create_info, nullptr, &fence.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan fence: {}", to_string(result));
    return std::unexpected {result};
  }

  return fence;
}

}  // namespace tactile
