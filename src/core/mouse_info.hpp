#pragma once

#include <centurion.hpp>  // mouse_button

#include "map_position.hpp"

namespace Tactile {

/// \addtogroup core
/// \{

struct MouseInfo final
{
  MapPosition mouse_position_in_map;
  cen::mouse_button button;
};

/// \} End of group core

}  // namespace Tactile
