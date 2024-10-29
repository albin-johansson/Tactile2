// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

/**
 * Represents a Vulkan allocator.
 */
class TACTILE_VULKAN_API VulkanAllocator final
{
 public:
  TACTILE_DELETE_COPY(VulkanAllocator);
  TACTILE_DECLARE_MOVE(VulkanAllocator);

  VulkanAllocator() = default;

  ~VulkanAllocator() noexcept;

  VmaAllocator handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

/**
 * Creates a Vulkan allocator.
 *
 * \param instance        The associated Vulkan instance.
 * \param physical_device The associated physical device.
 * \param device          The associated logical device.
 *
 * \return
 * A Vulkan allocator if successful; an error code otherwise.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_allocator(VkInstance instance,
                                                VkPhysicalDevice physical_device,
                                                VkDevice device)
    -> std::expected<VulkanAllocator, VkResult>;

}  // namespace tactile::vk
