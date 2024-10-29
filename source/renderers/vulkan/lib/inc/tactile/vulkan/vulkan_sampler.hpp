// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/base/render/renderer_options.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

/**
 * Represents a Vulkan texture sampler.
 */
class TACTILE_VULKAN_API VulkanSampler final
{
 public:
  TACTILE_DECLARE_MOVE(VulkanSampler);
  TACTILE_DELETE_COPY(VulkanSampler);

  VulkanSampler() = default;

  ~VulkanSampler() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkSampler handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

/**
 * Creates a Vulkan sampler.
 *
 * \param device  The associated Vulkan device.
 * \param options The renderer options to use.
 *
 * \return
 * A Vulkan sampler if successful; an error code otherwise.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_sampler(VkDevice device, const RendererOptions& options)
    -> std::expected<VulkanSampler, VkResult>;

}  // namespace tactile::vk
