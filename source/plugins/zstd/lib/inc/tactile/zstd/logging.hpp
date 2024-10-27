// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/log/logger.hpp"
#include "tactile/zstd/api.hpp"

namespace tactile::zstd {

TACTILE_ZSTD_API void set_logger(log::Logger* logger) noexcept;

[[nodiscard]]
TACTILE_ZSTD_API auto get_logger() noexcept -> log::Logger*;

}  // namespace tactile::zstd

#define TACTILE_ZSTD_TRACE(Fmt, ...) \
  TACTILE_LOG_TRACE(::tactile::zstd::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_ZSTD_DEBUG(Fmt, ...) \
  TACTILE_LOG_DEBUG(::tactile::zstd::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_ZSTD_INFO(Fmt, ...) \
  TACTILE_LOG_INFO(::tactile::zstd::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_ZSTD_WARN(Fmt, ...) \
  TACTILE_LOG_WARN(::tactile::zstd::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_ZSTD_ERROR(Fmt, ...) \
  TACTILE_LOG_ERROR(::tactile::zstd::get_logger(), Fmt, __VA_ARGS__)
