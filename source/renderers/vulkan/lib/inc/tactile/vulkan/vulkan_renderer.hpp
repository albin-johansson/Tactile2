// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstddef>        // size_t
#include <unordered_map>  // unordered_map
#include <vector>         // vector

#include <vulkan/vulkan.h>

#include "tactile/base/id.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/render/renderer.hpp"
#include "tactile/base/render/renderer_options.hpp"
#include "tactile/vulkan/api.hpp"
#include "tactile/vulkan/vulkan_allocator.hpp"
#include "tactile/vulkan/vulkan_command_buffer.hpp"
#include "tactile/vulkan/vulkan_command_pool.hpp"
#include "tactile/vulkan/vulkan_descriptor_pool.hpp"
#include "tactile/vulkan/vulkan_device.hpp"
#include "tactile/vulkan/vulkan_fence.hpp"
#include "tactile/vulkan/vulkan_imgui_context.hpp"
#include "tactile/vulkan/vulkan_instance.hpp"
#include "tactile/vulkan/vulkan_sampler.hpp"
#include "tactile/vulkan/vulkan_semaphore.hpp"
#include "tactile/vulkan/vulkan_surface.hpp"
#include "tactile/vulkan/vulkan_swapchain.hpp"
#include "tactile/vulkan/vulkan_texture.hpp"

namespace tactile::vk {

struct VulkanFrame final
{
  VulkanCommandBuffer command_buffer;
  VulkanFence in_flight_fence;
  VulkanSemaphore image_available_semaphore;
  VulkanSemaphore render_finished_semaphore;
};

class TACTILE_VULKAN_API VulkanRenderer final : public IRenderer
{
 public:
  TACTILE_DELETE_COPY(VulkanRenderer);
  TACTILE_DEFAULT_MOVE(VulkanRenderer);

  VulkanRenderer(const RendererOptions& options, IWindow* window);

  ~VulkanRenderer() noexcept override;

  [[nodiscard]]
  auto begin_frame() -> bool override;

  void end_frame() override;

  [[nodiscard]]
  auto load_texture(const std::filesystem::path& image_path)
      -> std::expected<TextureID, ErrorCode> override;

  void unload_texture(TextureID id) override;

  [[nodiscard]]
  auto find_texture(TextureID id) const -> const ITexture* override;

  void try_reload_fonts() override;

  [[nodiscard]]
  auto can_reload_fonts() const -> bool override;

  [[nodiscard]]
  auto get_window() -> IWindow* override;

  [[nodiscard]]
  auto get_window() const -> const IWindow* override;

  [[nodiscard]]
  auto get_imgui_context() -> ImGuiContext* override;

  void process_event(const SDL_Event& event) override;

  [[nodiscard]]
  auto get_options() -> const RendererOptions& override;

 private:
  RendererOptions m_options {};
  IWindow* m_window {};
  VulkanInstance m_instance {};
  VulkanSurface m_surface {};
  VkPhysicalDevice m_physical_device {};
  VulkanDevice m_device {};
  VulkanAllocator m_allocator {};
  VulkanSwapchain m_swapchain {};
  VulkanCommandPool m_graphics_command_pool {};
  VulkanSampler m_sampler {};
  VkQueue m_graphics_queue {VK_NULL_HANDLE};
  VkQueue m_present_queue {VK_NULL_HANDLE};
  VulkanDescriptorPool m_imgui_descriptor_pool {};
  PFN_vkCmdBeginRenderingKHR m_vkCmdBeginRendering {};
  PFN_vkCmdEndRenderingKHR m_vkCmdEndRendering {};
  std::vector<VulkanFrame> m_frames {};
  std::size_t m_frame_index {0};
  VulkanImGuiContext m_imgui_context {};
  std::unordered_map<TextureID, VulkanTexture> m_textures {};
  TextureID m_next_texture_id {1};

  void _record_commands() const;

  [[nodiscard]]
  static auto _reset_and_begin_command_buffer(const VulkanFrame& frame) -> VkResult;

  void _prepare_swapchain_image_for_rendering(const VulkanFrame& frame) const;

  void _begin_dynamic_rendering(const VulkanFrame& frame) const;

  void _render_content(const VulkanFrame& frame) const;

  void _end_dynamic_rendering(const VulkanFrame& frame) const;

  void _prepare_swapchain_image_for_presentation(const VulkanFrame& frame) const;

  static void _end_command_buffer(const VulkanFrame& frame);

  void _submit_commands();

  void _present_swapchain_image();

  [[nodiscard]]
  auto _recreate_swapchain() -> VkResult;
};

}  // namespace tactile::vk
