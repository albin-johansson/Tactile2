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

#include "remove_layer_cmd.hpp"

#include "core/systems/layers/layer_system.hpp"
#include "misc/assert.hpp"

namespace tactile {

RemoveLayerCmd::RemoveLayerCmd(RegistryRef registry, const LayerID id)
    : ACommand{"Remove Layer"}
    , mRegistry{registry}
    , mLayerId{id}
{}

void RemoveLayerCmd::undo()
{
  sys::restore_layer(mRegistry, mLayerSnapshot.value());
}

void RemoveLayerCmd::redo()
{
  const auto entity = sys::find_layer(mRegistry, mLayerId);
  TACTILE_ASSERT(entity != entt::null);

  mLayerSnapshot = sys::remove_layer(mRegistry, entity);
}

}  // namespace tactile
