// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_surface.hpp"

#include <utility>  // exchange

#include <SDL2/SDL_vulkan.h>

#include "tactile/base/render/window.hpp"
#include "tactile/vulkan/logging.hpp"

namespace tactile {

VulkanSurface::VulkanSurface(VulkanSurface&& other) noexcept
  : instance {std::exchange(other.instance, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanSurface::~VulkanSurface() noexcept
{
  _destroy();
}

auto VulkanSurface::operator=(VulkanSurface&& other) noexcept -> VulkanSurface&
{
  if (this != &other) {
    _destroy();

    instance = std::exchange(other.instance, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

void VulkanSurface::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(instance, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto create_vulkan_surface(VkInstance instance, IWindow& window)
    -> std::expected<VulkanSurface, VkResult>
{
  VulkanSurface surface {};
  surface.instance = instance;

  if (!SDL_Vulkan_CreateSurface(window.get_handle(), instance, &surface.handle)) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan surface: {}", SDL_GetError());
    return std::unexpected {VK_ERROR_UNKNOWN};
  }

  return surface;
}

}  // namespace tactile
