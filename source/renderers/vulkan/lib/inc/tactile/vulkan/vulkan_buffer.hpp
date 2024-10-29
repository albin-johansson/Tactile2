// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstdint>   // uint64_t
#include <expected>  // expected

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

class TACTILE_VULKAN_API VulkanBuffer final
{
 public:
  TACTILE_DELETE_COPY(VulkanBuffer);
  TACTILE_DECLARE_MOVE(VulkanBuffer);

  VulkanBuffer() = default;

  ~VulkanBuffer() noexcept;

  VmaAllocator allocator {VK_NULL_HANDLE};
  VkBuffer handle {VK_NULL_HANDLE};
  VmaAllocation allocation {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_buffer(VmaAllocator allocator,
                                             const VkBufferCreateInfo& buffer_info,
                                             const VmaAllocationCreateInfo& allocation_info)
    -> std::expected<VulkanBuffer, VkResult>;

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_staging_buffer(VmaAllocator allocator,
                                                     std::uint64_t size,
                                                     VkBufferUsageFlags usage_flags)
    -> std::expected<VulkanBuffer, VkResult>;

[[nodiscard]]
TACTILE_VULKAN_API auto set_buffer_data(VulkanBuffer& buffer,
                                        const void* data,
                                        std::uint64_t data_size) -> VkResult;

}  // namespace tactile::vk
