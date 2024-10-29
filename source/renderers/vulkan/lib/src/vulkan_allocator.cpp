// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_allocator.hpp"

#include <utility>  // exchange

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile::vk {

VulkanAllocator::VulkanAllocator(VulkanAllocator&& other) noexcept
  : handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanAllocator::~VulkanAllocator() noexcept
{
  _destroy();
}

void VulkanAllocator::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vmaDestroyAllocator(handle);
    handle = VK_NULL_HANDLE;
  }
}

auto VulkanAllocator::operator=(VulkanAllocator&& other) noexcept -> VulkanAllocator&
{
  if (this != &other) {
    _destroy();

    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

auto create_vulkan_allocator(VkInstance instance,
                             VkPhysicalDevice physical_device,
                             VkDevice device) -> std::expected<VulkanAllocator, VkResult>
{
  VmaVulkanFunctions functions {};
  functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

  const VmaAllocatorCreateInfo create_info {
    .flags = 0,
    .physicalDevice = physical_device,
    .device = device,
    .preferredLargeHeapBlockSize = 0,
    .pAllocationCallbacks = nullptr,
    .pDeviceMemoryCallbacks = nullptr,
    .pHeapSizeLimit = nullptr,
    .pVulkanFunctions = &functions,
    .instance = instance,
    .vulkanApiVersion = VK_API_VERSION_1_2,
    .pTypeExternalMemoryHandleTypes = nullptr,
  };

  VulkanAllocator allocator {};
  const auto result = vmaCreateAllocator(&create_info, &allocator.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan allocator: {}", to_string(result));
    return std::unexpected {result};
  }

  return allocator;
}

}  // namespace tactile::vk
