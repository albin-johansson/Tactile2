// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/base/render/window.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

/**
 * Represents a Vulkan surface.
 */
class TACTILE_VULKAN_API VulkanSurface final
{
 public:
  TACTILE_DELETE_COPY(VulkanSurface);
  TACTILE_DECLARE_MOVE(VulkanSurface);

  VulkanSurface() = default;

  ~VulkanSurface() noexcept;

  VkInstance instance {VK_NULL_HANDLE};
  VkSurfaceKHR handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

/**
 * Creates a Vulkan surface.
 *
 * \param instance The associated Vulkan instance.
 * \param window   The target window.
 *
 * \return
 * A Vulkan surface if successful; an error code otherwise.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_surface(VkInstance instance, IWindow& window)
    -> std::expected<VulkanSurface, VkResult>;

}  // namespace tactile::vk
