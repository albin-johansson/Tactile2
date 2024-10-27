// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_shader_module.hpp"

#include <utility>  // exchange

#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile {

VulkanShaderModule::VulkanShaderModule(VulkanShaderModule&& other) noexcept
  : device {std::exchange(other.device, VK_NULL_HANDLE)},
    handle {std::exchange(other.handle, VK_NULL_HANDLE)}
{}

VulkanShaderModule::~VulkanShaderModule() noexcept
{
  _destroy();
}

void VulkanShaderModule::_destroy() noexcept
{
  if (handle != VK_NULL_HANDLE) {
    vkDestroyShaderModule(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

auto VulkanShaderModule::operator=(VulkanShaderModule&& other) noexcept -> VulkanShaderModule&
{
  if (this != &other) {
    _destroy();

    device = std::exchange(other.device, VK_NULL_HANDLE);
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
  }

  return *this;
}

auto create_vulkan_shader_module(VkDevice device, std::span<const std::uint32_t> code)
    -> std::expected<VulkanShaderModule, VkResult>
{
  const VkShaderModuleCreateInfo create_info {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .codeSize = code.size_bytes(),
    .pCode = code.data(),
  };

  VulkanShaderModule shader_module {};
  shader_module.device = device;

  const auto result =
      vkCreateShaderModule(device, &create_info, nullptr, &shader_module.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan shader module: {}", to_string(result));
    return std::unexpected {result};
  }

  return shader_module;
}

}  // namespace tactile
