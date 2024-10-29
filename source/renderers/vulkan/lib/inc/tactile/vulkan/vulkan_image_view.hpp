// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstdint>   // uint32_t
#include <expected>  // expected

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

class TACTILE_VULKAN_API VulkanImageView final
{
 public:
  TACTILE_DELETE_COPY(VulkanImageView);
  TACTILE_DECLARE_MOVE(VulkanImageView);

  VulkanImageView() = default;

  ~VulkanImageView() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkImageView handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_image_view(VkDevice device,
                                                 VkImage image,
                                                 VkFormat format,
                                                 VkImageViewType type,
                                                 VkImageAspectFlags aspect_mask,
                                                 std::uint32_t mip_levels)
    -> std::expected<VulkanImageView, VkResult>;

}  // namespace tactile::vk
