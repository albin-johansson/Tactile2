#pragma once

#include <centurion.hpp>  // mouse_button

#include <tactile-base/map_position.hpp>

namespace Tactile {

/// \addtogroup core
/// \{

struct MouseInfo final
{
  MapPosition position_in_map;
  cen::mouse_button button;
};

/// \} End of group core

}  // namespace Tactile
