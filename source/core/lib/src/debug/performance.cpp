// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/debug/performance.hpp"

#include <exception>  // exception

#include "tactile/core/logging.hpp"

namespace tactile::core {

ScopeProfiler::ScopeProfiler(const char* description) noexcept
  : mDescription {description ? description : "?"},
    mStartInstant {std::chrono::steady_clock::now()}
{}

ScopeProfiler::~ScopeProfiler() noexcept
{
  try {
    const auto end = std::chrono::steady_clock::now();
    const auto duration = duration_cast<std::chrono::microseconds>(end - mStartInstant);
    TACTILE_CORE_DEBUG("Scope '{}' took {}", mDescription, duration);
  }
  catch (const std::exception& e) {
    TACTILE_CORE_ERROR("Scope profiler destructor threw unexpected exception: {}", e.what());
  }
}

}  // namespace tactile::core
