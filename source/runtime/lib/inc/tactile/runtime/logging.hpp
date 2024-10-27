// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/log/logger.hpp"
#include "tactile/runtime/api.hpp"

namespace tactile::runtime {

TACTILE_RUNTIME_API void set_logger(log::Logger* logger) noexcept;

[[nodiscard]]
TACTILE_RUNTIME_API auto get_logger() noexcept -> log::Logger*;

}  // namespace tactile::runtime

#define TACTILE_RUNTIME_TRACE(Fmt, ...) \
  TACTILE_LOG_TRACE(::tactile::runtime::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_RUNTIME_DEBUG(Fmt, ...) \
  TACTILE_LOG_DEBUG(::tactile::runtime::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_RUNTIME_INFO(Fmt, ...) \
  TACTILE_LOG_INFO(::tactile::runtime::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_RUNTIME_WARN(Fmt, ...) \
  TACTILE_LOG_WARN(::tactile::runtime::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_RUNTIME_ERROR(Fmt, ...) \
  TACTILE_LOG_ERROR(::tactile::runtime::get_logger(), Fmt, __VA_ARGS__)
