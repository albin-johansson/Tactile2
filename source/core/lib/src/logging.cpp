// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/logging.hpp"

namespace tactile::core {
namespace {

inline constinit log::Logger* gLogger {};

}  // namespace

void set_logger(log::Logger* logger) noexcept
{
  gLogger = logger;
}

auto get_logger() noexcept -> log::Logger*
{
  return gLogger;
}

}  // namespace tactile::core
