// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/log/logger.hpp"
#include "tactile/opengl/api.hpp"

namespace tactile::gl {

TACTILE_OPENGL_API void set_logger(log::Logger* logger) noexcept;

[[nodiscard]]
TACTILE_OPENGL_API auto get_logger() noexcept -> log::Logger*;

}  // namespace tactile::gl

#define TACTILE_OPENGL_TRACE(Fmt, ...) \
  TACTILE_LOG_TRACE(::tactile::gl::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_OPENGL_DEBUG(Fmt, ...) \
  TACTILE_LOG_DEBUG(::tactile::gl::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_OPENGL_INFO(Fmt, ...) \
  TACTILE_LOG_INFO(::tactile::gl::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_OPENGL_WARN(Fmt, ...) \
  TACTILE_LOG_WARN(::tactile::gl::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_OPENGL_ERROR(Fmt, ...) \
  TACTILE_LOG_ERROR(::tactile::gl::get_logger(), Fmt, __VA_ARGS__)
