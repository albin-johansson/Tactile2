// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstdint>  // uint8_t

namespace tactile::log {

/**
 * The supported log level categories.
 *
 * \details
 * The underlying value increases with severity.
 */
enum class LogLevel : std::uint8_t
{
  /** For verbose messages only useful for detailed debugging. */
  kTrace,

  /** For messages that provide useful developer information. */
  kDebug,

  /** For messages that provide general high-level information. */
  kInfo,

  /** For messages that warn about potential issues. */
  kWarn,

  /** For messages that report errors. */
  kError,
};

}  // namespace tactile
