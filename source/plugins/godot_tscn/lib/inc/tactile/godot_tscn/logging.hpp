// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/godot_tscn/api.hpp"
#include "tactile/log/logger.hpp"

namespace tactile::godot_tscn {

TACTILE_GODOT_API void set_logger(log::Logger* logger) noexcept;

[[nodiscard]]
TACTILE_GODOT_API auto get_logger() noexcept -> log::Logger*;

}  // namespace tactile::godot_tscn

#define TACTILE_GODOT_TSCN_TRACE(Fmt, ...) \
  TACTILE_LOG_TRACE(::tactile::godot_tscn::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_GODOT_TSCN_DEBUG(Fmt, ...) \
  TACTILE_LOG_DEBUG(::tactile::godot_tscn::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_GODOT_TSCN_INFO(Fmt, ...) \
  TACTILE_LOG_INFO(::tactile::godot_tscn::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_GODOT_TSCN_WARN(Fmt, ...) \
  TACTILE_LOG_WARN(::tactile::godot_tscn::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_GODOT_TSCN_ERROR(Fmt, ...) \
  TACTILE_LOG_ERROR(::tactile::godot_tscn::get_logger(), Fmt, __VA_ARGS__)
