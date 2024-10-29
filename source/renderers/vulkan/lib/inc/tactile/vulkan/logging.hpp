// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/log/logger.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

TACTILE_VULKAN_API void set_logger(log::Logger* logger) noexcept;

[[nodiscard]]
TACTILE_VULKAN_API auto get_logger() noexcept -> log::Logger*;

}  // namespace tactile::vk

#define TACTILE_VULKAN_TRACE(Fmt, ...) \
  TACTILE_LOG_TRACE(::tactile::vk::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_VULKAN_DEBUG(Fmt, ...) \
  TACTILE_LOG_DEBUG(::tactile::vk::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_VULKAN_INFO(Fmt, ...) \
  TACTILE_LOG_INFO(::tactile::vk::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_VULKAN_WARN(Fmt, ...) \
  TACTILE_LOG_WARN(::tactile::vk::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_VULKAN_ERROR(Fmt, ...) \
  TACTILE_LOG_ERROR(::tactile::vk::get_logger(), Fmt, __VA_ARGS__)
