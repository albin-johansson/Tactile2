// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_device.hpp"

#include <set>      // set
#include <utility>  // exchange
#include <vector>   // vector

#include "tactile/base/render/renderer_options.hpp"
#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_physical_device.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile::vk {
namespace {

[[nodiscard]]
auto _get_device_extensions() -> std::vector<const char*>
{
  std::vector<const char*> enabled_extensions {};
  enabled_extensions.reserve(3);

  enabled_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  enabled_extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

#ifdef TACTILE_USE_VULKAN_SUBSET
  enabled_extensions.push_back("VK_KHR_portability_subset");
#endif  // TACTILE_USE_VULKAN_SUBSET

  return enabled_extensions;
}

}  // namespace

VulkanDevice::VulkanDevice(VulkanDevice&& other) noexcept
  : handle {std::exchange(other.handle, VK_NULL_HANDLE)},
    graphics_queue_family {std::exchange(other.graphics_queue_family, 0)},
    presentation_queue_family {std::exchange(other.presentation_queue_family, 0)}
{}

VulkanDevice::~VulkanDevice() noexcept
{
  _destroy();
}

void VulkanDevice::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroyDevice(handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto VulkanDevice::operator=(VulkanDevice&& other) noexcept -> VulkanDevice&
{
  if (this != &other) {
    _destroy();

    handle = std::exchange(other.handle, VK_NULL_HANDLE);
    graphics_queue_family = std::exchange(other.graphics_queue_family, 0);
    presentation_queue_family = std::exchange(other.presentation_queue_family, 0);
  }

  return *this;
}

auto create_vulkan_device(VkPhysicalDevice physical_device,
                          VkSurfaceKHR surface,
                          const RendererOptions& options)
    -> std::expected<VulkanDevice, VkResult>
{
  const auto queue_family_indices = get_queue_family_indices(physical_device, surface);

  if (!queue_family_indices.graphics.has_value() ||
      !queue_family_indices.present.has_value()) {
    TACTILE_VULKAN_ERROR("Missing Vulkan graphics or presentation queue");
    return std::unexpected {VK_ERROR_UNKNOWN};
  }

  const std::set unique_queue_family_indices = {
    queue_family_indices.graphics.value(),
    queue_family_indices.present.value(),
  };

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos {};
  queue_create_infos.reserve(unique_queue_family_indices.size());

  constexpr float queue_priority = 1.0f;

  for (const auto queue_family_index : unique_queue_family_indices) {
    queue_create_infos.push_back(VkDeviceQueueCreateInfo {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = queue_family_index,
      .queueCount = 1,
      .pQueuePriorities = &queue_priority,
    });
  }

  std::vector<const char*> enabled_layers {};
  if (options.vulkan_validation) {
    enabled_layers.push_back("VK_LAYER_KHRONOS_validation");
  }

  const auto enabled_extensions = _get_device_extensions();
  for (const auto* extension : enabled_extensions) {
    TACTILE_VULKAN_DEBUG("Using Vulkan device extension '{}'", extension);
  }

  constexpr VkPhysicalDeviceFeatures enabled_features {};

  constexpr VkPhysicalDeviceDynamicRenderingFeaturesKHR enabled_dynamic_rendering_features {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
    .pNext = nullptr,
    .dynamicRendering = VK_TRUE,
  };

  const VkDeviceCreateInfo device_info {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = &enabled_dynamic_rendering_features,
    .flags = 0,
    .queueCreateInfoCount = static_cast<std::uint32_t>(queue_create_infos.size()),
    .pQueueCreateInfos = queue_create_infos.data(),
    .enabledLayerCount = static_cast<std::uint32_t>(enabled_layers.size()),
    .ppEnabledLayerNames = enabled_layers.data(),
    .enabledExtensionCount = static_cast<std::uint32_t>(enabled_extensions.size()),
    .ppEnabledExtensionNames = enabled_extensions.data(),
    .pEnabledFeatures = &enabled_features,
  };

  VulkanDevice device {};
  device.graphics_queue_family = *queue_family_indices.graphics;
  device.presentation_queue_family = *queue_family_indices.present;

  const auto result = vkCreateDevice(physical_device, &device_info, nullptr, &device.handle);
  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan device: {}", to_string(result));
    return std::unexpected {result};
  }

  return device;
}

}  // namespace tactile::vk
