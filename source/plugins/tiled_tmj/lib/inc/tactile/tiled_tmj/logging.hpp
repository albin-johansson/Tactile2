// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "api.hpp"
#include "tactile/log/logger.hpp"
#include "tactile/tiled_tmj/api.hpp"

namespace tactile::tiled_tmj {

TACTILE_TILED_TMJ_API void set_logger(log::Logger* logger) noexcept;

[[nodiscard]]
TACTILE_TILED_TMJ_API auto get_logger() noexcept -> log::Logger*;

}  // namespace tactile::tiled_tmj

#define TACTILE_TILED_TMJ_TRACE(Fmt, ...) \
  TACTILE_LOG_TRACE(::tactile::tiled_tmj::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_TILED_TMJ_DEBUG(Fmt, ...) \
  TACTILE_LOG_DEBUG(::tactile::tiled_tmj::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_TILED_TMJ_INFO(Fmt, ...) \
  TACTILE_LOG_INFO(::tactile::tiled_tmj::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_TILED_TMJ_WARN(Fmt, ...) \
  TACTILE_LOG_WARN(::tactile::tiled_tmj::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_TILED_TMJ_ERROR(Fmt, ...) \
  TACTILE_LOG_ERROR(::tactile::tiled_tmj::get_logger(), Fmt, __VA_ARGS__)
