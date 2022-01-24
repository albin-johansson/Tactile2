#pragma once

#include <centurion.hpp>  // mouse_button

#include "core/map_position.hpp"

namespace tactile {

/// \addtogroup core
/// \{

struct MouseInfo final
{
  float x{};
  float y{};
  map_position position_in_map;
  cen::mouse_button button{};
};

/// \} End of group core

}  // namespace tactile
