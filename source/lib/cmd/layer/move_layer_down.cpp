/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2023 Albin Johansson
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

#include "move_layer_down.hpp"

#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/context.hpp"
#include "model/systems/group_layer_system.hpp"

namespace tactile::cmd {

MoveLayerDown::MoveLayerDown(Entity map_entity, Entity layer_entity)
    : mMapEntity {map_entity},
      mLayerEntity {layer_entity}
{
}

void MoveLayerDown::undo()
{
  auto& model = get_global_model();
  sys::move_layer_up(model, mMapEntity, mLayerEntity);
}

void MoveLayerDown::redo()
{
  auto& model = get_global_model();
  sys::move_layer_down(model, mMapEntity, mLayerEntity);
}

auto MoveLayerDown::get_name() const -> String
{
  const auto& lang = get_current_language();
  return lang.cmd.move_layer_down;
}

}  // namespace tactile::cmd
