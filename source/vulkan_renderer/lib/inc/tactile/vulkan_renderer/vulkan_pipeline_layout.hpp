// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan_renderer/api.hpp"

namespace tactile {

class TACTILE_VULKAN_API VulkanPipelineLayout final
{
 public:
  TACTILE_DELETE_COPY(VulkanPipelineLayout);
  TACTILE_DECLARE_MOVE(VulkanPipelineLayout);

  VulkanPipelineLayout() = default;

  ~VulkanPipelineLayout() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkPipelineLayout handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

}  // namespace tactile
