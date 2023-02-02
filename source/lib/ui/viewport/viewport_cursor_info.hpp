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

#pragma once

#include <imgui.h>

#include "common/macros.hpp"
#include "core/tile/tile_pos.hpp"

TACTILE_FWD_DECLARE_STRUCT_NS(tactile, CanvasInfo)

namespace tactile::ui {

struct ViewportCursorInfo final {
  TilePos map_position;
  ImVec2 clamped_position {};
  ImVec2 raw_position {};
  ImVec2 scaled_position {};
  bool is_within_map {};
};

[[nodiscard]] auto get_viewport_cursor_info(const CanvasInfo& canvas_info)
    -> ViewportCursorInfo;

}  // namespace tactile::ui
