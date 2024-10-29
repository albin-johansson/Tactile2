// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <optional>  // optional

#include "tactile/base/prelude.hpp"
#include "tactile/base/render/window.hpp"
#include "tactile/base/util/scope_exit.hpp"
#include "tactile/vulkan/api.hpp"
#include "tactile/vulkan/vulkan_descriptor_set_layout.hpp"
#include "tactile/vulkan/vulkan_pipeline.hpp"
#include "tactile/vulkan/vulkan_pipeline_layout.hpp"

struct ImGui_ImplVulkan_InitInfo;
struct ImGuiContext;

namespace tactile::vk {

class TACTILE_VULKAN_API VulkanImGuiContext final
{
 public:
  TACTILE_DELETE_COPY(VulkanImGuiContext);
  TACTILE_DEFAULT_MOVE(VulkanImGuiContext);

  VulkanImGuiContext() = default;

  [[nodiscard]]
  static auto init(IWindow& window, ImGui_ImplVulkan_InitInfo& vulkan_info)
      -> std::optional<VulkanImGuiContext>;

  [[nodiscard]]
  auto get() const -> ImGuiContext*;

  [[nodiscard]]
  auto get_pipeline() const -> VkPipeline;

 private:
  ImGuiContext* m_ctx {};
  ScopeExit m_context_deleter {};
  ScopeExit m_backend_impl_deleter {};
  ScopeExit m_renderer_impl_deleter {};
  VulkanDescriptorSetLayout m_descriptor_set_layout {};
  VulkanPipelineLayout m_pipeline_layout {};
  VulkanPipeline m_pipeline {};
};

}  // namespace tactile::vk
