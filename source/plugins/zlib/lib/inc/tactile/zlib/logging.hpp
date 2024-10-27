// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/log/logger.hpp"
#include "tactile/zlib/api.hpp"

namespace tactile::zlib {

TACTILE_ZLIB_API void set_logger(log::Logger* logger) noexcept;

[[nodiscard]]
TACTILE_ZLIB_API auto get_logger() noexcept -> log::Logger*;

}  // namespace tactile::zlib

#define TACTILE_ZLIB_TRACE(Fmt, ...) \
  TACTILE_LOG_TRACE(::tactile::zlib::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_ZLIB_DEBUG(Fmt, ...) \
  TACTILE_LOG_DEBUG(::tactile::zlib::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_ZLIB_INFO(Fmt, ...) \
  TACTILE_LOG_INFO(::tactile::zlib::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_ZLIB_WARN(Fmt, ...) \
  TACTILE_LOG_WARN(::tactile::zlib::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_ZLIB_ERROR(Fmt, ...) \
  TACTILE_LOG_ERROR(::tactile::zlib::get_logger(), Fmt, __VA_ARGS__)
