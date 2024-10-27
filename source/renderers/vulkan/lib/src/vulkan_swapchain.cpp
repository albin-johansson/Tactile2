// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_swapchain.hpp"

#include <algorithm>  // clamp
#include <limits>     // numeric_limits
#include <utility>    // move, exchange

#include <magic_enum.hpp>

#include "tactile/base/render/renderer_options.hpp"
#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_physical_device.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile {
namespace {

[[nodiscard]]
auto _pick_image_format(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
    -> std::expected<VkSurfaceFormatKHR, VkResult>
{
  std::vector<VkSurfaceFormatKHR> surface_formats {};

  std::uint32_t surface_format_count {};
  auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
                                                     surface,
                                                     &surface_format_count,
                                                     nullptr);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not get Vulkan surface format count: {}", to_string(result));
    return std::unexpected {result};
  }

  surface_formats.resize(surface_format_count);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
                                                surface,
                                                &surface_format_count,
                                                surface_formats.data());

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not get Vulkan surface formats: {}", to_string(result));
    return std::unexpected {result};
  }

  for (const auto& format : surface_formats) {
    if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR &&
        (format.format == VK_FORMAT_B8G8R8A8_SRGB ||
         format.format == VK_FORMAT_R8G8B8A8_SRGB)) {
      return format;
    }
  }

  return surface_formats.at(0);
}

[[nodiscard]]
auto _get_preferred_present_mode(const RendererOptions& options) -> VkPresentModeKHR
{
  if (!options.use_vsync) {
    return VK_PRESENT_MODE_IMMEDIATE_KHR;
  }

  if (!options.limit_fps) {
    return VK_PRESENT_MODE_MAILBOX_KHR;
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

[[nodiscard]]
auto _pick_present_mode(VkPhysicalDevice physical_device,
                        VkSurfaceKHR surface,
                        const RendererOptions& options)
    -> std::expected<VkPresentModeKHR, VkResult>
{
  std::vector<VkPresentModeKHR> present_modes {};

  std::uint32_t present_mode_count {};
  auto result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                                          surface,
                                                          &present_mode_count,
                                                          nullptr);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not get Vulkan present mode count: {}", to_string(result));
    return std::unexpected {result};
  }

  present_modes.resize(present_mode_count);
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                                     surface,
                                                     &present_mode_count,
                                                     present_modes.data());

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not get Vulkan present modes: {}", to_string(result));
    return std::unexpected {result};
  }

  const auto preferred_present_mode = _get_preferred_present_mode(options);

  TACTILE_VULKAN_TRACE("Preferred presentation mode is '{}'",
                       magic_enum::enum_name(preferred_present_mode));

  for (const auto present_mode : present_modes) {
    if (present_mode == preferred_present_mode) {
      return present_mode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

[[nodiscard]]
auto _get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain)
    -> std::expected<std::vector<VkImage>, VkResult>
{
  std::vector<VkImage> images {};

  std::uint32_t image_count {};
  auto result = vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not get Vulkan swapchain image count: {}", to_string(result));
    return std::unexpected {result};
  }

  images.resize(image_count);
  result = vkGetSwapchainImagesKHR(device, swapchain, &image_count, images.data());

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not get Vulkan swapchain images: {}", to_string(result));
    return std::unexpected {result};
  }

  return images;
}

void _prepare_depth_buffer(VkDevice device,
                           VmaAllocator allocator,
                           const VkFormat format,
                           const VkExtent2D extent,
                           VulkanImage& depth_buffer,
                           VulkanImageView& depth_buffer_view)
{
  const VulkanImageParams depth_buffer_params {
    .type = VK_IMAGE_TYPE_2D,
    .format = format,
    .layout = VK_IMAGE_LAYOUT_UNDEFINED,
    .extent = extent,
    .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
             VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    .mip_levels = 1,
  };

  depth_buffer = create_vulkan_image(allocator, depth_buffer_params).value();
  depth_buffer_view = create_vulkan_image_view(device,
                                               depth_buffer.handle,
                                               depth_buffer_params.format,
                                               VK_IMAGE_VIEW_TYPE_2D,
                                               VK_IMAGE_ASPECT_DEPTH_BIT,
                                               1)
                          .value();
}

}  // namespace

VulkanSwapchain::VulkanSwapchain(VulkanSwapchain&& other) noexcept
  : surface {std::exchange(other.surface, VK_NULL_HANDLE)},
    device {std::exchange(other.device, VK_NULL_HANDLE)},
    allocator {std::exchange(other.allocator, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)},
    params {std::exchange(other.params, VulkanSwapchainParams {})},
    images {std::exchange(other.images, {})},
    image_views {std::exchange(other.image_views, {})},
    depth_buffer {std::exchange(other.depth_buffer, VulkanImage {})},
    depth_buffer_view {std::exchange(other.depth_buffer_view, VulkanImageView {})},
    image_index {std::exchange(other.image_index, 0)}
{}

VulkanSwapchain::~VulkanSwapchain() noexcept
{
  _destroy();
}

auto VulkanSwapchain::operator=(VulkanSwapchain&& other) noexcept -> VulkanSwapchain&
{
  if (this != &other) {
    _destroy();

    surface = std::exchange(other.surface, VK_NULL_HANDLE);
    device = std::exchange(other.device, VK_NULL_HANDLE);
    allocator = std::exchange(other.allocator, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
    params = std::exchange(other.params, VulkanSwapchainParams {});
    images = std::exchange(other.images, {});
    image_views = std::exchange(other.image_views, {});
    depth_buffer = std::exchange(other.depth_buffer, VulkanImage {});
    depth_buffer_view = std::exchange(other.depth_buffer_view, VulkanImageView {});
    image_index = std::exchange(other.image_index, 0);
  }

  return *this;
}

void VulkanSwapchain::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto create_vulkan_swapchain(VkSurfaceKHR surface,
                             VkDevice device,
                             VmaAllocator allocator,
                             VulkanSwapchainParams params,
                             VkSwapchainKHR old_swapchain)
    -> std::expected<VulkanSwapchain, VkResult>
{
  const VkSwapchainCreateInfoKHR swapchain_info {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = nullptr,
    .flags = 0,
    .surface = surface,
    .minImageCount = params.min_image_count,
    .imageFormat = params.image_format,
    .imageColorSpace = params.image_color_space,
    .imageExtent = params.image_extent,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .imageSharingMode = params.image_sharing_mode,
    .queueFamilyIndexCount = static_cast<std::uint32_t>(params.queue_family_indices.size()),
    .pQueueFamilyIndices = params.queue_family_indices.data(),
    .preTransform = params.pre_transform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = params.present_mode,
    .clipped = VK_TRUE,
    .oldSwapchain = old_swapchain,
  };

  VulkanSwapchain swapchain {};
  swapchain.surface = surface;
  swapchain.device = device;
  swapchain.allocator = allocator;
  swapchain.params = std::move(params);

  const auto result =
      vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &swapchain.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan swapchain: {}", to_string(result));
    return std::unexpected {result};
  }

  if (auto images = _get_swapchain_images(device, swapchain.handle)) {
    swapchain.images = std::move(*images);
  }
  else {
    return std::unexpected {images.error()};
  }

  swapchain.image_views.reserve(swapchain.images.size());
  for (VkImage image : swapchain.images) {
    if (auto image_view = create_vulkan_image_view(device,
                                                   image,
                                                   swapchain.params.image_format,
                                                   VK_IMAGE_VIEW_TYPE_2D,
                                                   VK_IMAGE_ASPECT_COLOR_BIT,
                                                   1)) {
      swapchain.image_views.push_back(std::move(*image_view));
    }
    else {
      return std::unexpected {image_view.error()};
    }
  }

  _prepare_depth_buffer(device,
                        allocator,
                        swapchain.params.depth_format,
                        swapchain.params.image_extent,
                        swapchain.depth_buffer,
                        swapchain.depth_buffer_view);

  return swapchain;
}

auto create_vulkan_swapchain(VkSurfaceKHR surface,
                             VkPhysicalDevice physical_device,
                             VkDevice device,
                             VmaAllocator allocator,
                             const RendererOptions& options)
    -> std::expected<VulkanSwapchain, VkResult>
{
  const auto queue_family_indices = get_queue_family_indices(physical_device, surface);
  auto unique_queue_family_indices = to_unique(queue_family_indices);

  VkSurfaceCapabilitiesKHR capabilities {};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);

  const auto image_width = std::clamp(capabilities.currentExtent.width,
                                      capabilities.minImageExtent.width,
                                      capabilities.maxImageExtent.width);

  const auto image_height = std::clamp(capabilities.currentExtent.height,
                                       capabilities.minImageExtent.height,
                                       capabilities.maxImageExtent.height);

  const auto min_image_count = std::clamp(capabilities.minImageCount + 1,
                                          capabilities.minImageCount,
                                          capabilities.maxImageCount);

  const auto image_sharing_mode = queue_family_indices.graphics != queue_family_indices.present
                                      ? VK_SHARING_MODE_CONCURRENT
                                      : VK_SHARING_MODE_EXCLUSIVE;

  TACTILE_VULKAN_DEBUG("Using image sharing mode '{}'",
                       magic_enum::enum_name(image_sharing_mode));

  const auto surface_format = _pick_image_format(physical_device, surface);
  if (!surface_format.has_value()) {
    return std::unexpected {surface_format.error()};
  }

  TACTILE_VULKAN_DEBUG("Using surface format '{}'",
                       magic_enum::enum_name(surface_format->format));

  TACTILE_VULKAN_DEBUG("Using surface color space '{}'",
                       magic_enum::enum_name(surface_format->colorSpace));

  const auto present_mode = _pick_present_mode(physical_device, surface, options);
  if (!present_mode.has_value()) {
    return std::unexpected {present_mode.error()};
  }

  TACTILE_VULKAN_DEBUG("Using presentation mode '{}'", magic_enum::enum_name(*present_mode));

  const VulkanSwapchainParams params {
    .image_extent = VkExtent2D {image_width, image_height},
    .image_format = surface_format->format,
    .depth_format = VK_FORMAT_D32_SFLOAT,
    .image_color_space = surface_format->colorSpace,
    .image_sharing_mode = image_sharing_mode,
    .present_mode = *present_mode,
    .pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    .min_image_count = min_image_count,
    .queue_family_indices = std::move(unique_queue_family_indices),
  };

  return create_vulkan_swapchain(surface, device, allocator, params);
}

}  // namespace tactile
