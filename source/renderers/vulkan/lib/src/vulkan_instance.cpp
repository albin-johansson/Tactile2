// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_instance.hpp"

#include <cstdint>  // uint32_t
#include <vector>   // vector

#include <SDL2/SDL_vulkan.h>

#include "tactile/base/render/renderer_options.hpp"
#include "tactile/base/render/window.hpp"
#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile {
namespace {

[[nodiscard]]
auto _get_required_extensions(IWindow& window) -> std::vector<const char*>
{
  std::uint32_t extension_count = 0;
  SDL_Vulkan_GetInstanceExtensions(window.get_handle(), &extension_count, nullptr);

  std::vector<const char*> extensions {};
  extensions.resize(extension_count);

  SDL_Vulkan_GetInstanceExtensions(window.get_handle(), &extension_count, extensions.data());

#ifdef TACTILE_USE_VULKAN_SUBSET
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // TACTILE_USE_VULKAN_SUBSET

  return extensions;
}

}  // namespace

void VulkanInstanceDeleter::operator()(VkInstance instance) const noexcept
{
  vkDestroyInstance(instance, nullptr);
}

auto create_vulkan_instance(IWindow& window, const RendererOptions& options)
    -> std::expected<VulkanInstance, VkResult>
{
  constexpr auto tactile_version =
      VK_MAKE_VERSION(TACTILE_MAJOR_VERSION, TACTILE_MINOR_VERSION, TACTILE_PATCH_VERSION);

  constexpr VkApplicationInfo app_info {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = "Tactile",
    .applicationVersion = tactile_version,
    .pEngineName = "Tactile Vulkan Renderer",
    .engineVersion = tactile_version,
    .apiVersion = VK_API_VERSION_1_2,
  };

  std::vector<const char*> enabled_layers {};
  if (options.vulkan_validation) {
    enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
  }

  const auto enabled_extensions = _get_required_extensions(window);

  VkInstanceCreateInfo instance_info {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .pApplicationInfo = &app_info,
    .enabledLayerCount = static_cast<std::uint32_t>(enabled_layers.size()),
    .ppEnabledLayerNames = enabled_layers.data(),
    .enabledExtensionCount = static_cast<std::uint32_t>(enabled_extensions.size()),
    .ppEnabledExtensionNames = enabled_extensions.data(),
  };

#ifdef TACTILE_USE_VULKAN_SUBSET
  instance_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif  // TACTILE_USE_VULKAN_SUBSET

  for (const auto* layer : enabled_layers) {
    TACTILE_VULKAN_DEBUG("Using Vulkan layer '{}'", layer);
  }

  for (const auto* extension : enabled_extensions) {
    TACTILE_VULKAN_DEBUG("Using Vulkan instance extension '{}'", extension);
  }

  VkInstance instance {VK_NULL_HANDLE};
  const auto result = vkCreateInstance(&instance_info, nullptr, &instance);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan instance: {}", to_string(result));
    return std::unexpected {result};
  }

  return VulkanInstance {instance};
}

}  // namespace tactile
