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

#include "bucket_cmd.hpp"

#include "core/algorithms/flood_fill.hpp"
#include "core/components/layer.hpp"
#include "core/systems/layers/layer_system.hpp"
#include "core/systems/layers/tile_layer_system.hpp"
#include "misc/assert.hpp"

namespace tactile {

bucket_cmd::bucket_cmd(registry_ref registry,
                       const tile_position origin,
                       const tile_id replacement)
    : command_base{"Bucket Fill"}
    , mRegistry{registry}
    , mLayer{sys::get_active_layer_id(registry).value()}
    , mOrigin{origin}
    , mReplacement{replacement}
{}

void bucket_cmd::undo()
{
  auto& registry = mRegistry.get();

  const auto entity = sys::find_layer(registry, mLayer);
  TACTILE_ASSERT(entity != entt::null);
  TACTILE_ASSERT(registry.all_of<comp::tile_layer>(entity));

  const auto target = mTarget.value();
  auto& layer = registry.get<comp::tile_layer>(entity);
  for (const auto& position : mPositions) {
    layer.matrix.at(position.row_index()).at(position.col_index()) = target;
  }

  mPositions.clear();
  mTarget.reset();
}

void bucket_cmd::redo()
{
  auto& registry = mRegistry.get();

  const auto entity = sys::get_tile_layer_entity(registry, mLayer);
  const auto& layer = registry.get<comp::tile_layer>(entity);

  mTarget = sys::get_tile(layer, mOrigin);
  flood(registry, entity, mOrigin, mReplacement, mPositions);
}

}  // namespace tactile
