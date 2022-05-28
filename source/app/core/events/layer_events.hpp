/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>  // string

#include "core/common/identifiers.hpp"
#include "core/layer_type.hpp"

namespace tactile {

/// \addtogroup events
/// \{

struct AddLayerEvent final
{
  LayerType type{};
};

struct RemoveLayerEvent final
{
  LayerID id{};
};

struct DuplicateLayerEvent final
{
  LayerID id{};
};

struct SelectLayerEvent final
{
  LayerID id{};
};

struct MoveLayerUpEvent final
{
  LayerID id{};
};

struct MoveLayerDownEvent final
{
  LayerID id{};
};

struct SetLayerOpacityEvent final
{
  LayerID id{};
  float opacity{};
};

struct SetLayerVisibleEvent final
{
  LayerID id{};
  bool visible{};
};

struct OpenRenameLayerDialogEvent final
{
  LayerID id{};
};

struct RenameLayerEvent final
{
  LayerID id{};
  std::string name;
};

/// \} End of group events

}  // namespace tactile