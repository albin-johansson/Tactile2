// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_pipeline_layout.hpp"

#include <utility>  // exchange

namespace tactile::vk {

VulkanPipelineLayout::VulkanPipelineLayout(VulkanPipelineLayout&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanPipelineLayout::~VulkanPipelineLayout() noexcept
{
  _destroy();
}

auto VulkanPipelineLayout::operator=(VulkanPipelineLayout&& other) noexcept
    -> VulkanPipelineLayout&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

void VulkanPipelineLayout::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

}  // namespace tactile::vk
