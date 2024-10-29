// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <concepts>  // invocable
#include <cstdint>   // uint32_t
#include <expected>  // expected

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"
#include "tactile/vulkan/vulkan_command_buffer.hpp"

namespace tactile::vk {

class TACTILE_VULKAN_API VulkanCommandPool final
{
 public:
  TACTILE_DECLARE_MOVE(VulkanCommandPool);
  TACTILE_DELETE_COPY(VulkanCommandPool);

  VulkanCommandPool() = default;

  ~VulkanCommandPool() noexcept;

  VkDevice device {VK_NULL_HANDLE};
  VkCommandPool handle {VK_NULL_HANDLE};

 private:
  void _destroy() noexcept;
};

[[nodiscard]]
TACTILE_VULKAN_API auto create_vulkan_command_pool(VkDevice device,
                                                   std::uint32_t queue_family_index,
                                                   VkCommandPoolCreateFlags flags)
    -> std::expected<VulkanCommandPool, VkResult>;

template <std::invocable<VkCommandBuffer> T>
[[nodiscard]] auto record_and_submit_commands(VkDevice device,
                                              VkQueue queue,
                                              VkCommandPool command_pool,
                                              const T& callable) -> VkResult
{
  auto command_buffer = create_vulkan_command_buffer(device, command_pool);
  if (!command_buffer.has_value()) {
    return command_buffer.error();
  }

  constexpr VkCommandBufferBeginInfo begin_info {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = nullptr,
    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    .pInheritanceInfo = nullptr,
  };

  auto result = vkBeginCommandBuffer(command_buffer->handle, &begin_info);
  if (result != VK_SUCCESS) {
    return result;
  }

  callable(command_buffer->handle);

  result = vkEndCommandBuffer(command_buffer->handle);
  if (result != VK_SUCCESS) {
    return result;
  }

  const VkSubmitInfo submit_info {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = nullptr,
    .waitSemaphoreCount = 0,
    .pWaitSemaphores = nullptr,
    .pWaitDstStageMask = nullptr,
    .commandBufferCount = 1,
    .pCommandBuffers = &command_buffer->handle,
    .signalSemaphoreCount = 0,
    .pSignalSemaphores = nullptr,
  };

  result = vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
  if (result != VK_SUCCESS) {
    return result;
  }

  result = vkQueueWaitIdle(queue);
  return result;
}

}  // namespace tactile::vk
