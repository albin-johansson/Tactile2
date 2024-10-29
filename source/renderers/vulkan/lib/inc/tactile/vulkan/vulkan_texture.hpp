// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>    // expected
#include <filesystem>  // path

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/base/render/renderer_options.hpp"
#include "tactile/base/render/texture.hpp"
#include "tactile/vulkan/api.hpp"
#include "tactile/vulkan/vulkan_image.hpp"
#include "vulkan_image_view.hpp"

namespace tactile::vk {

class TACTILE_VULKAN_API VulkanTexture final : public ITexture
{
 public:
  [[nodiscard]]
  auto get_handle() const -> void* override;

  [[nodiscard]]
  auto get_size() const -> TextureSize override;

  [[nodiscard]]
  auto get_path() const -> const std::filesystem::path& override;

  VulkanImage image {};
  VulkanImageView view {};
  std::filesystem::path path {};
  void* imgui_handle {};
};

[[nodiscard]]
TACTILE_VULKAN_API auto load_vulkan_texture(VkDevice device,
                                            VkQueue queue,
                                            VkCommandPool command_pool,
                                            VmaAllocator allocator,
                                            VkSampler sampler,
                                            const std::filesystem::path& image_path,
                                            const RendererOptions& options)
    -> std::expected<VulkanTexture, VkResult>;

}  // namespace tactile::vk
