// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

class TACTILE_VULKAN_API VulkanPipeline final
{
 public:
  TACTILE_DELETE_COPY(VulkanPipeline);
  TACTILE_DECLARE_MOVE(VulkanPipeline);

  VulkanPipeline() = default;

  ~VulkanPipeline() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkPipeline handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

}  // namespace tactile::vk
