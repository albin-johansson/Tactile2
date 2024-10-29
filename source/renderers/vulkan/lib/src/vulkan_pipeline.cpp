// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_pipeline.hpp"

#include <utility>  // exchange

#include <imgui.h>

namespace tactile::vk {

VulkanPipeline::VulkanPipeline(VulkanPipeline&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanPipeline::~VulkanPipeline() noexcept
{
  _destroy();
}

auto VulkanPipeline::operator=(VulkanPipeline&& other) noexcept -> VulkanPipeline&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

void VulkanPipeline::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroyPipeline(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

}  // namespace tactile::vk
