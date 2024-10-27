// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "api.hpp"
#include "tactile/log/logger.hpp"
#include "tactile/yaml/api.hpp"

namespace tactile::yaml_format {

TACTILE_YAML_FORMAT_API void set_logger(log::Logger* logger) noexcept;

[[nodiscard]]
TACTILE_YAML_FORMAT_API auto get_logger() noexcept -> log::Logger*;

}  // namespace tactile::yaml_format

#define TACTILE_YAML_FORMAT_TRACE(Fmt, ...) \
  TACTILE_LOG_TRACE(::tactile::yaml_format::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_YAML_FORMAT_DEBUG(Fmt, ...) \
  TACTILE_LOG_DEBUG(::tactile::yaml_format::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_YAML_FORMAT_INFO(Fmt, ...) \
  TACTILE_LOG_INFO(::tactile::yaml_format::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_YAML_FORMAT_WARN(Fmt, ...) \
  TACTILE_LOG_WARN(::tactile::yaml_format::get_logger(), Fmt, __VA_ARGS__)

#define TACTILE_YAML_FORMAT_ERROR(Fmt, ...) \
  TACTILE_LOG_ERROR(::tactile::yaml_format::get_logger(), Fmt, __VA_ARGS__)
