// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstdint>   // uint32_t
#include <expected>  // expected

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/base/render/renderer_options.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

class TACTILE_VULKAN_API VulkanDevice final
{
 public:
  TACTILE_DELETE_COPY(VulkanDevice);
  TACTILE_DECLARE_MOVE(VulkanDevice);

  VulkanDevice() = default;

  ~VulkanDevice() noexcept;

  VkDevice handle {VK_NULL_HANDLE};
  std::uint32_t graphics_queue_family {};
  std::uint32_t presentation_queue_family {};

 private:
  void _destroy() noexcept;
};

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_device(VkPhysicalDevice physical_device,
                                             VkSurfaceKHR surface,
                                             const RendererOptions& options)
    -> std::expected<VulkanDevice, VkResult>;

}  // namespace tactile::vk
