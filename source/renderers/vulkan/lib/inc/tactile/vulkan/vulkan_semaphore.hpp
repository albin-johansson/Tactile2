// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

/**
 * Represents a Vulkan semaphore synchronization primitive.
 */
class TACTILE_VULKAN_API VulkanSemaphore final
{
 public:
  TACTILE_DELETE_COPY(VulkanSemaphore);
  TACTILE_DECLARE_MOVE(VulkanSemaphore);

  VulkanSemaphore() = default;

  ~VulkanSemaphore() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkSemaphore handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

/**
 * Creates a Vulkan semaphore.
 *
 * \param device The associated Vulkan device.
 *
 * \return
 * A Vulkan semaphore if successful; an error code otherwise.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_semaphore(VkDevice device)
    -> std::expected<VulkanSemaphore, VkResult>;

}  // namespace tactile::vk
