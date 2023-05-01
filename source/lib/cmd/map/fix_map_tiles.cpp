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

#include "fix_map_tiles.hpp"

#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/model.hpp"

namespace tactile::cmd {

FixMapTiles::FixMapTiles(const Entity map_entity)
    : mMapEntity {map_entity}
{
}

void FixMapTiles::undo()
{
  auto& model = get_model();
  sys::restore_tiles_in_map(model, mInvalidLayerTiles);
  mInvalidLayerTiles.clear();
}

void FixMapTiles::redo()
{
  auto& model = get_model();
  mInvalidLayerTiles = sys::fix_tiles_in_map(model, mMapEntity);
}

auto FixMapTiles::get_name() const -> String
{
  const auto& lang = get_current_language();
  return lang.cmd.fix_map_tiles;
}

}  // namespace tactile::cmd
