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

#include "render_tile_layer.hpp"

#include "core/layer/tile_layer.hpp"
#include "core/region.hpp"
#include "editor/ui/render/graphics.hpp"
#include "editor/ui/render/render_tile.hpp"

namespace tactile::ui {

void render_tile_layer(GraphicsCtx& graphics,
                       const Map& map,
                       const TileLayer& layer,
                       const float parent_opacity)
{
  graphics.set_opacity(parent_opacity * layer.get_opacity());

  const auto bounds = graphics.bounds();
  const auto end_row = bounds.end.row();
  const auto end_col = bounds.end.col();

  for (auto row = bounds.begin.row(); row < end_row; ++row) {
    for (auto col = bounds.begin.col(); col < end_col; ++col) {
      const TilePos pos {row, col};
      const auto tile = layer.tile_at(pos);
      if (tile != empty_tile) {
        render_tile(graphics, map, tile, pos);
      }
    }
  }
}

}  // namespace tactile::ui
