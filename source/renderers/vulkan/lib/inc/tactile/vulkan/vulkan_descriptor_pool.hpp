// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

class TACTILE_VULKAN_API VulkanDescriptorPool final
{
 public:
  TACTILE_DELETE_COPY(VulkanDescriptorPool);
  TACTILE_DECLARE_MOVE(VulkanDescriptorPool);

  VulkanDescriptorPool() noexcept = default;

  ~VulkanDescriptorPool() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkDescriptorPool handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_imgui_descriptor_pool(VkDevice device)
    -> std::expected<VulkanDescriptorPool, VkResult>;

}  // namespace tactile::vk
