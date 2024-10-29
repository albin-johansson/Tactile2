// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstdint>   // uint32_t
#include <expected>  // expected
#include <vector>    // vector

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/base/render/renderer_options.hpp"
#include "tactile/vulkan/api.hpp"
#include "tactile/vulkan/vulkan_image.hpp"
#include "tactile/vulkan/vulkan_image_view.hpp"

namespace tactile::vk {

struct VulkanSwapchainParams final
{
  VkExtent2D image_extent;
  VkFormat image_format;
  VkFormat depth_format;
  VkColorSpaceKHR image_color_space;
  VkSharingMode image_sharing_mode;
  VkPresentModeKHR present_mode;
  VkSurfaceTransformFlagBitsKHR pre_transform;
  std::uint32_t min_image_count;
  std::vector<std::uint32_t> queue_family_indices;
};

class TACTILE_VULKAN_API VulkanSwapchain final
{
 public:
  TACTILE_DELETE_COPY(VulkanSwapchain);
  TACTILE_DECLARE_MOVE(VulkanSwapchain);

  VulkanSwapchain() = default;

  ~VulkanSwapchain() noexcept;

  VkSurfaceKHR surface {VK_NULL_HANDLE};
  VkDevice device {VK_NULL_HANDLE};
  VmaAllocator allocator {VK_NULL_HANDLE};
  VkSwapchainKHR handle {VK_NULL_HANDLE};
  VulkanSwapchainParams params {};
  std::vector<VkImage> images {};
  std::vector<VulkanImageView> image_views {};
  VulkanImage depth_buffer {};
  VulkanImageView depth_buffer_view {};
  std::uint32_t image_index {0};

 private:
  void _destroy() noexcept;
};

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_swapchain(VkSurfaceKHR surface,
                                                VkDevice device,
                                                VmaAllocator allocator,
                                                VulkanSwapchainParams params,
                                                VkSwapchainKHR old_swapchain = VK_NULL_HANDLE)
    -> std::expected<VulkanSwapchain, VkResult>;

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_swapchain(VkSurfaceKHR surface,
                                                VkPhysicalDevice physical_device,
                                                VkDevice device,
                                                VmaAllocator allocator,
                                                const RendererOptions& options)
    -> std::expected<VulkanSwapchain, VkResult>;

}  // namespace tactile::vk
