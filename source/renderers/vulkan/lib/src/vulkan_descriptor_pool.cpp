// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_descriptor_pool.hpp"

#include <utility>  // exchange

#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile::vk {

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDescriptorPool&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanDescriptorPool::~VulkanDescriptorPool() noexcept
{
  _destroy();
}

auto VulkanDescriptorPool::operator=(VulkanDescriptorPool&& other) noexcept
    -> VulkanDescriptorPool&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

void VulkanDescriptorPool::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroyDescriptorPool(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto create_vulkan_imgui_descriptor_pool(VkDevice device)
    -> std::expected<VulkanDescriptorPool, VkResult>
{
  constexpr VkDescriptorPoolSize pool_sizes[] = {
    VkDescriptorPoolSize {
      .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 8,
    },
  };

  const VkDescriptorPoolCreateInfo descriptor_pool_info {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
    .maxSets = 64,
    .poolSizeCount = 1,
    .pPoolSizes = pool_sizes,
  };

  VulkanDescriptorPool descriptor_pool {};
  descriptor_pool.device = device;

  const auto result =
      vkCreateDescriptorPool(device, &descriptor_pool_info, nullptr, &descriptor_pool.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan ImGui descriptor pool: {}",
                         to_string(result));
    return std::unexpected {result};
  }

  return descriptor_pool;
}

}  // namespace tactile::vk
