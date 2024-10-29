// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

/**
 * Represents a Vulkan fence synchronization primitive.
 */
class TACTILE_VULKAN_API VulkanFence final
{
 public:
  TACTILE_DECLARE_MOVE(VulkanFence);
  TACTILE_DELETE_COPY(VulkanFence);

  VulkanFence() = default;

  ~VulkanFence() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkFence handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

/**
 * Creates a Vulkan fence.
 *
 * \param device The associated Vulkan device.
 * \param flags  The fence creation flags.
 *
 * \return
 * A Vulkan fence if successful; an error code otherwise.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_fence(VkDevice device, VkFenceCreateFlags flags)
    -> std::expected<VulkanFence, VkResult>;

}  // namespace tactile::vk
