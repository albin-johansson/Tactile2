// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

class VulkanCommandBuffer final
{
 public:
  TACTILE_DECLARE_MOVE(VulkanCommandBuffer);
  TACTILE_DELETE_COPY(VulkanCommandBuffer);

  VulkanCommandBuffer() = default;

  ~VulkanCommandBuffer() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkCommandPool pool {VK_NULL_HANDLE};
  VkCommandBuffer handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_command_buffer(VkDevice device, VkCommandPool pool)
    -> std::expected<VulkanCommandBuffer, VkResult>;

}  // namespace tactile::vk
