// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <imgui_impl_vulkan.h>
#include <stb_image.h>

#include "tactile/base/io/file_io.hpp"
#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/base/render/renderer_options.hpp"
#include "tactile/base/util/scope_exit.hpp"
#include "tactile/vulkan/vulkan_buffer.hpp"

namespace tactile::vk {

auto VulkanTexture::get_handle() const -> void*
{
  return imgui_handle;
}

auto VulkanTexture::get_size() const -> TextureSize
{
  return TextureSize {saturate_cast<int>(image.params.extent.width),
                      saturate_cast<int>(image.params.extent.height)};
}

auto VulkanTexture::get_path() const -> const std::filesystem::path&
{
  return path;
}

auto load_vulkan_texture(VkDevice device,
                         VkQueue queue,
                         VkCommandPool command_pool,
                         VmaAllocator allocator,
                         VkSampler sampler,
                         const std::filesystem::path& image_path,
                         const RendererOptions& options)
    -> std::expected<VulkanTexture, VkResult>
{
  int width {};
  int height {};
  int channels {};
  auto* pixels =
      stbi_load(image_path.string().c_str(), &width, &height, &channels, STBI_default);

  if (!pixels) {
    return std::unexpected {VK_ERROR_UNKNOWN};
  }

  const auto pixel_bytes = width * height * channels;
  const ScopeExit pixels_deleter {[pixels] { stbi_image_free(pixels); }};

  auto staging_buffer = create_vulkan_staging_buffer(allocator, pixel_bytes, 0);
  if (!staging_buffer.has_value()) {
    return std::unexpected {staging_buffer.error()};
  }

  VkFormat format {VK_FORMAT_UNDEFINED};
  switch (channels) {
    case STBI_rgb:        format = VK_FORMAT_R8G8B8_UNORM; break;
    case STBI_rgb_alpha:  format = VK_FORMAT_R8G8B8A8_UNORM; break;
    case STBI_grey:       [[fallthrough]];
    case STBI_grey_alpha: [[fallthrough]];
    default:              return std::unexpected {VK_ERROR_FORMAT_NOT_SUPPORTED};
  }

  const VkExtent2D image_extent {static_cast<std::uint32_t>(width),
                                 static_cast<std::uint32_t>(height)};

  auto image = create_vulkan_image(
      allocator,
      VulkanImageParams {
        .type = VK_IMAGE_TYPE_2D,
        .format = format,
        .layout = VK_IMAGE_LAYOUT_UNDEFINED,
        .extent = image_extent,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                 VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .mip_levels = calculate_vulkan_mip_levels(image_extent),
      });
  if (!image.has_value()) {
    return std::unexpected {image.error()};
  }

  auto result = set_buffer_data(*staging_buffer, pixels, pixel_bytes);
  if (result != VK_SUCCESS) {
    return std::unexpected {result};
  }

  result =
      image->change_layout(device, queue, command_pool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  if (result != VK_SUCCESS) {
    return std::unexpected {result};
  }

  result = image->copy_buffer(device, queue, command_pool, staging_buffer->handle);
  if (result != VK_SUCCESS) {
    return std::unexpected {result};
  }

  if (options.use_mipmaps) {
    result = image->generate_mipmaps(device, queue, command_pool);
    if (result != VK_SUCCESS) {
      return std::unexpected {result};
    }
  }

  result = image->change_layout(device,
                                queue,
                                command_pool,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  if (result != VK_SUCCESS) {
    return std::unexpected {result};
  }

  auto image_view = create_vulkan_image_view(device,
                                             image->handle,
                                             image->params.format,
                                             VK_IMAGE_VIEW_TYPE_2D,
                                             VK_IMAGE_ASPECT_COLOR_BIT,
                                             1);
  if (!image_view.has_value()) {
    return std::unexpected {image_view.error()};
  }

  VulkanTexture texture {};
  texture.path = image_path;
  texture.image = std::move(*image);
  texture.view = std::move(*image_view);

  texture.imgui_handle =
      ImGui_ImplVulkan_AddTexture(sampler, texture.view.handle, texture.image.params.layout);
  if (texture.imgui_handle == VK_NULL_HANDLE) {
    return std::unexpected {VK_ERROR_INITIALIZATION_FAILED};
  }

  return texture;
}

}  // namespace tactile::vk
