// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan_renderer/vulkan_physical_device.hpp"

#include <algorithm>      // sort, unique, max_element
#include <unordered_map>  // unordered_map

#include "tactile/runtime/logging.hpp"

namespace tactile {

auto list_physical_devices(VkInstance instance) -> std::vector<VkPhysicalDevice>
{
  std::uint32_t gpu_count {0};
  vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);

  std::vector<VkPhysicalDevice> gpus {};
  gpus.resize(gpu_count);

  vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data());

  return gpus;
}

auto get_queue_families(VkPhysicalDevice physical_device)
    -> std::vector<VkQueueFamilyProperties>
{
  std::uint32_t queue_family_count {0};
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families {};
  queue_families.resize(queue_family_count);

  vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                           &queue_family_count,
                                           queue_families.data());

  return queue_families;
}

auto get_queue_family_indices(VkPhysicalDevice physical_device,
                              VkSurfaceKHR surface) -> VulkanQueueFamilyIndices
{
  VulkanQueueFamilyIndices indices {};

  std::uint32_t index {0};
  for (const auto& queue_family : get_queue_families(physical_device)) {
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics = index;
    }

    VkBool32 has_present_support = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device,
                                         index,
                                         surface,
                                         &has_present_support);

    if (has_present_support == VK_TRUE) {
      indices.present = index;
    }

    ++index;
  }

  return indices;
}

auto to_unique(const VulkanQueueFamilyIndices& family_indices) -> std::vector<std::uint32_t>
{
  std::vector unique_indices = {
    family_indices.graphics.value(),
    family_indices.present.value(),
  };

  std::ranges::sort(unique_indices);

  const auto duplicated_range = std::ranges::unique(unique_indices);
  unique_indices.erase(duplicated_range.begin(), duplicated_range.end());

  return unique_indices;
}

auto get_surface_formats(VkPhysicalDevice physical_device,
                         VkSurfaceKHR surface) -> std::vector<VkSurfaceFormatKHR>
{
  std::uint32_t format_count {0};
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);

  std::vector<VkSurfaceFormatKHR> formats {};
  formats.resize(format_count);

  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
                                       surface,
                                       &format_count,
                                       formats.data());

  return formats;
}

auto get_present_modes(VkPhysicalDevice physical_device,
                       VkSurfaceKHR surface) -> std::vector<VkPresentModeKHR>
{
  std::uint32_t present_mode_count {0};
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                            surface,
                                            &present_mode_count,
                                            nullptr);

  std::vector<VkPresentModeKHR> present_modes {};
  present_modes.resize(present_mode_count);

  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                            surface,
                                            &present_mode_count,
                                            present_modes.data());

  return present_modes;
}

auto pick_physical_device(VkInstance instance,
                          VkSurfaceKHR surface) -> std::expected<VkPhysicalDevice, VkResult>
{
  auto physical_devices = list_physical_devices(instance);

  if (physical_devices.empty()) {
    log(LogLevel::kError, "Found no physical devices with Vulkan support");
    return std::unexpected {VK_ERROR_UNKNOWN};
  }

  std::erase_if(physical_devices, [surface](VkPhysicalDevice physical_device) {
    const auto queue_family_indices = get_queue_family_indices(physical_device, surface);

    if (!queue_family_indices.present.has_value() ||
        !queue_family_indices.graphics.has_value()) {
      return true;
    }

    if (get_surface_formats(physical_device, surface).empty()) {
      return true;
    }

    return get_present_modes(physical_device, surface).empty();
  });

  if (physical_devices.empty()) {
    return std::unexpected {VK_ERROR_UNKNOWN};
  }

  std::unordered_map<VkPhysicalDevice, int> physical_device_scores {};
  physical_device_scores.reserve(physical_devices.size());

  for (VkPhysicalDevice physical_device : physical_devices) {
    VkPhysicalDeviceProperties physical_device_properties {};
    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

    int score {0};
    if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score = 2;
    }
    else if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      score = 1;
    }
    else if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
      score = -1;
    }

    physical_device_scores[physical_device] = score;
  }

  const auto max_elem_iter =
      std::ranges::max_element(physical_device_scores, [](const auto& x, const auto& y) {
        const auto x_score = x.second;
        const auto y_score = y.second;
        return x_score < y_score;
      });

  if (max_elem_iter == physical_device_scores.end()) {
    log(LogLevel::kError, "Could not find a suitable physical device for Vulkan");
    return std::unexpected {VK_ERROR_UNKNOWN};
  }

  VkPhysicalDeviceProperties final_properties {};
  vkGetPhysicalDeviceProperties(max_elem_iter->first, &final_properties);

  log(LogLevel::kDebug,
      "Using VkPhysicalDevice '{}'",
      static_cast<const char*>(final_properties.deviceName));

  return max_elem_iter->first;
}

}  // namespace tactile