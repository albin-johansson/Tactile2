// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_descriptor_set_layout.hpp"

#include <utility>  // exchange

namespace tactile::vk {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
    VulkanDescriptorSetLayout&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() noexcept
{
  _destroy();
}

auto VulkanDescriptorSetLayout::operator=(VulkanDescriptorSetLayout&& other) noexcept
    -> VulkanDescriptorSetLayout&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

void VulkanDescriptorSetLayout::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroyDescriptorSetLayout(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

}  // namespace tactile::vk
