// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <algorithm>  // max
#include <cmath>      // floor, log2
#include <cstdint>    // uint32_t, uint64_t
#include <expected>   // expected

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

struct VulkanImageParams final
{
  VkImageType type;
  VkFormat format;
  VkImageLayout layout;
  VkExtent2D extent;
  VkImageUsageFlags usage;
  std::uint32_t mip_levels;
};

class TACTILE_VULKAN_API VulkanImage final
{
 public:
  TACTILE_DELETE_COPY(VulkanImage);
  TACTILE_DECLARE_MOVE(VulkanImage);

  VulkanImage() = default;

  ~VulkanImage() noexcept;

  VmaAllocator allocator {VK_NULL_HANDLE};
  VkImage handle {VK_NULL_HANDLE};
  VmaAllocation allocation {VK_NULL_HANDLE};
  VulkanImageParams params {};

  [[nodiscard]]
  auto change_layout(VkDevice device,
                     VkQueue queue,
                     VkCommandPool command_pool,
                     VkImageLayout new_layout) -> VkResult;

  [[nodiscard]]
  auto set_data(VkDevice device,
                VkQueue queue,
                VkCommandPool command_pool,
                const void* data,
                std::uint64_t data_size) -> VkResult;

  [[nodiscard]]
  auto copy_buffer(VkDevice device, VkQueue queue, VkCommandPool command_pool, VkBuffer buffer)
      -> VkResult;

  [[nodiscard]]
  auto generate_mipmaps(VkDevice device, VkQueue queue, VkCommandPool command_pool)
      -> VkResult;

 private:
  void _destroy() noexcept;
};

[[nodiscard]]
inline auto calculate_vulkan_mip_levels(const VkExtent2D extent) -> std::uint32_t
{
  const auto max_extent = std::max(extent.width, extent.height);
  return 1u + static_cast<std::uint32_t>(std::floor(std::log2(max_extent)));
}

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_image(VmaAllocator allocator,
                                            const VulkanImageParams& params)
    -> std::expected<VulkanImage, VkResult>;

}  // namespace tactile::vk
