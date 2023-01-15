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

#include "render_info.hpp"

#include <algorithm>  // min, max

#include <imgui_internal.h>

#include "core/map.hpp"
#include "core/tile/tileset.hpp"
#include "core/viewport.hpp"
#include "ui/conversions.hpp"

namespace tactile::ui {
namespace {

[[nodiscard]] auto get_render_bounds(const ImVec2& tl,
                                     const ImVec2& br,
                                     const ImVec2& origin,
                                     const ImVec2& grid_size,
                                     const float rows,
                                     const float cols) -> Region
{
  const auto begin = (tl - origin) / grid_size;
  const auto end = (br - origin) / grid_size;

  const auto begin_row = std::max(0, static_cast<int32>(begin.y));
  const auto begin_col = std::max(0, static_cast<int32>(begin.x));

  const auto end_row = static_cast<int32>(std::min(rows, end.y + 1));
  const auto end_col = static_cast<int32>(std::min(cols, end.x + 1));

  Region bounds;

  bounds.begin = {begin_row, begin_col};
  bounds.end = {end_row, end_col};

  return bounds;
}

[[nodiscard]] auto get_render_info(const Viewport& viewport,
                                   const ImVec2& logical_tile_size,
                                   const TileExtent extent) -> RenderInfo
{
  RenderInfo info;

  info.canvas_tl = ImGui::GetCursorScreenPos();
  info.canvas_br = info.canvas_tl + ImGui::GetContentRegionAvail();
  info.canvas_size = info.canvas_br - info.canvas_tl;

  info.origin = info.canvas_tl + from_vec(viewport.get_offset());

  info.tile_size = logical_tile_size;
  info.grid_size = from_vec(viewport.tile_size());
  info.ratio = info.grid_size / info.tile_size;

  const auto tiles_in_viewport = info.canvas_size / info.grid_size;
  info.tiles_in_viewport_x = static_cast<int32>(tiles_in_viewport.x) + 1;
  info.tiles_in_viewport_y = static_cast<int32>(tiles_in_viewport.y) + 1;

  info.row_count = static_cast<float>(extent.rows);
  info.col_count = static_cast<float>(extent.cols);

  info.contents_size = ImVec2 {info.col_count, info.row_count} * info.grid_size;

  info.bounds = get_render_bounds(info.canvas_tl,
                                  info.canvas_br,
                                  info.origin,
                                  info.grid_size,
                                  info.row_count,
                                  info.col_count);

  return info;
}

}  // namespace

auto get_render_info(const Viewport& viewport, const Map& map) -> RenderInfo
{
  return get_render_info(viewport, from_vec(map.get_tile_size()), map.get_extent());
}

auto get_render_info(const Viewport& viewport, const Tileset& tileset) -> RenderInfo
{
  const TileExtent extent {static_cast<usize>(tileset.row_count()),
                           static_cast<usize>(tileset.column_count())};
  return get_render_info(viewport, from_vec(tileset.tile_size()), extent);
}

}  // namespace tactile::ui