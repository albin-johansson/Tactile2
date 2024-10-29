// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstdint>   // uint32_t
#include <expected>  // expected
#include <span>      // span

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

/**
 * Represents a Vulkan shader module.
 */
class TACTILE_VULKAN_API VulkanShaderModule final
{
 public:
  TACTILE_DELETE_COPY(VulkanShaderModule);
  TACTILE_DECLARE_MOVE(VulkanShaderModule);

  VulkanShaderModule() = default;

  ~VulkanShaderModule() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkShaderModule handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

/**
 * Creates a Vulkan shader module from a compiled shader.
 *
 * \param device The associated Vulkan device.
 * \param code   The compiled shader module code.
 *
 * \return
 * A Vulkan shader module if successful; an error code otherwise.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_shader_module(VkDevice device,
                                                    std::span<const std::uint32_t> code)
    -> std::expected<VulkanShaderModule, VkResult>;

}  // namespace tactile::vk
