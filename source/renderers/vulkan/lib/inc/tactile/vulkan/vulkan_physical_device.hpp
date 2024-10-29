// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstdint>   // uint32_t
#include <expected>  // expected
#include <optional>  // optional
#include <vector>    // vector

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

struct VulkanQueueFamilyIndices final
{
  /** The graphics family index, if any. */
  std::optional<std::uint32_t> graphics;

  /** The presentation family index, if any. */
  std::optional<std::uint32_t> present;
};

/**
 * Returns all physical devices available to a given Vulkan instance.
 *
 * \param instance The Vulkan instance.
 *
 * \return
 * The available physical devices.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto get_physical_devices(VkInstance instance)
    -> std::vector<VkPhysicalDevice>;

/**
 * Returns information about the queue families associated with a given physical device.
 *
 * \param physical_device The physical device.
 *
 * \return
 * A collection of queue family descriptions.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto get_queue_families(VkPhysicalDevice physical_device)
    -> std::vector<VkQueueFamilyProperties>;

/**
 * Returns the indices of the relevant queue families for a given physical device.
 *
 * \param physical_device The physical device.
 * \param surface         The target surface.
 *
 * \return
 * The queue family indices.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto get_queue_family_indices(VkPhysicalDevice physical_device,
                                                 VkSurfaceKHR surface)
    -> VulkanQueueFamilyIndices;

/**
 * Returns a set of distinct queue family indices.
 *
 * \param family_indices The queue family indices.
 *
 * \return
 * A collection of queue family indices without duplicates.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto to_unique(const VulkanQueueFamilyIndices& family_indices)
    -> std::vector<std::uint32_t>;

/**
 * Returns the supported surface formats for a given physical device.
 *
 * \param physical_device The physical device.
 * \param surface         The target surface.
 *
 * \return
 * A collection of surface formats.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto get_surface_formats(VkPhysicalDevice physical_device,
                                            VkSurfaceKHR surface)
    -> std::vector<VkSurfaceFormatKHR>;

/**
 * Returns the supported present modes for a given physical device.
 *
 * \param physical_device The physical device.
 * \param surface         The target surface.
 *
 * \return
 * A collection of present modes.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto get_present_modes(VkPhysicalDevice physical_device,
                                          VkSurfaceKHR surface)
    -> std::vector<VkPresentModeKHR>;

/**
 * Attempts to select a suitable physical device to use.
 *
 * \param instance The associated Vulkan instance.
 * \param surface  The target surface.
 *
 * \return
 * A selected physical device if successful; an error code otherwise.
 */
[[nodiscard]]
TACTILE_VULKAN_API auto select_physical_device(VkInstance instance, VkSurfaceKHR surface)
    -> std::expected<VkPhysicalDevice, VkResult>;

}  // namespace tactile::vk
