// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_sampler.hpp"

#include <utility>  // exchange

#include <magic_enum.hpp>

#include "tactile/base/render/renderer_options.hpp"
#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile {

VulkanSampler::VulkanSampler(VulkanSampler&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanSampler::~VulkanSampler() noexcept
{
  _destroy();
}

void VulkanSampler::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroySampler(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto VulkanSampler::operator=(VulkanSampler&& other) noexcept -> VulkanSampler&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

auto create_vulkan_sampler(VkDevice device, const RendererOptions& options)
    -> std::expected<VulkanSampler, VkResult>
{
  const auto filter_mode = options.texture_filter_mode == TextureFilterMode::kNearest
                               ? VK_FILTER_NEAREST
                               : VK_FILTER_LINEAR;

  const auto mipmap_filter_mode = options.texture_filter_mode == TextureFilterMode::kNearest
                                      ? VK_SAMPLER_MIPMAP_MODE_NEAREST
                                      : VK_SAMPLER_MIPMAP_MODE_LINEAR;

  TACTILE_VULKAN_DEBUG("Using sampler filter mode '{}'", magic_enum::enum_name(filter_mode));
  TACTILE_VULKAN_DEBUG("Using sampler mipmap filter mode '{}'",
                       magic_enum::enum_name(mipmap_filter_mode));

  const VkSamplerCreateInfo create_info {
    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .magFilter = filter_mode,
    .minFilter = filter_mode,
    .mipmapMode = mipmap_filter_mode,
    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .mipLodBias = 0.0f,
    .anisotropyEnable = VK_FALSE,
    .maxAnisotropy = 0.0f,
    .compareEnable = VK_FALSE,
    .compareOp = VK_COMPARE_OP_NEVER,
    .minLod = 0.0f,
    .maxLod = 64.0f,
    .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    .unnormalizedCoordinates = VK_FALSE,
  };

  VulkanSampler sampler {};
  sampler.device = device;

  const auto result = vkCreateSampler(device, &create_info, nullptr, &sampler.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan sampler: {}", to_string(result));
    return std::unexpected {result};
  }

  return sampler;
}

}  // namespace tactile
