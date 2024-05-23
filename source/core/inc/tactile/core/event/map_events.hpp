// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/prelude.hpp"
#include "tactile/core/map/map_spec.hpp"

namespace tactile {

/**
 * Event for creating new map documents.
 *
 * \ingroup Event
 */
struct CreateMapEvent final
{
  /** The requested map specification. */
  MapSpec spec;
};

}  // namespace tactile
