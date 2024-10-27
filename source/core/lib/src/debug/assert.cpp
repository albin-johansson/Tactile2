// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/debug/assert.hpp"

#include <exception>  // terminate

#include "tactile/core/logging.hpp"

namespace tactile::core {

void on_assertion_failed(const char* expr, const char* msg, const char* file, const int line)
{
  TACTILE_CORE_ERROR("{}:{} expression '{}' is false: {}", file, line, expr, msg ? msg : "?");
  std::terminate();
}

}  // namespace tactile::core
