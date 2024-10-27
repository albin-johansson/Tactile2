// Copyright (C) 2023 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/debug/terminate.hpp"

#include <cstdlib>  // abort

#include "tactile/core/debug/stacktrace.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core {

void on_terminate() noexcept
{
  try {
    const auto trace = get_stacktrace();
    TACTILE_CORE_ERROR("Into exile I must go. Failed I have.\n{}", trace);
  }
  catch (...) {  // NOLINT(*-empty-catch)
    // Not much we can do.
  }

  std::abort();
}

}  // namespace tactile::core
