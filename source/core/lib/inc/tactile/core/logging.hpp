// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/log/logger.hpp"

namespace tactile::core {

void set_logger(log::Logger* logger) noexcept;

[[nodiscard]]
auto get_logger() noexcept -> log::Logger*;

}  // namespace tactile::core

#define TACTILE_CORE_TRACE(Fmt, ...) \
  TACTILE_LOG_TRACE(::tactile::core::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_CORE_DEBUG(Fmt, ...) \
  TACTILE_LOG_DEBUG(::tactile::core::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_CORE_INFO(Fmt, ...) \
  TACTILE_LOG_INFO(::tactile::core::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_CORE_WARN(Fmt, ...) \
  TACTILE_LOG_WARN(::tactile::core::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_CORE_ERROR(Fmt, ...) \
  TACTILE_LOG_ERROR(::tactile::core::get_logger(), Fmt, __VA_ARGS__)
