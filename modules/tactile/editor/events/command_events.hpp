#pragma once

#include "tactile_def.hpp"

namespace tactile {

/// \addtogroup events
/// \{

struct UndoEvent final
{};

struct RedoEvent final
{};

struct SetCommandCapacityEvent final
{
  usize capacity{};
};

/// \} End of group events

}  // namespace tactile
