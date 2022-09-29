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

#include "core/common/fs.hpp"
#include "core/common/math.hpp"
#include "core/common/vocabulary.hpp"
#include "core/region.hpp"
#include "core/uuid.hpp"

namespace tactile {

struct ShowTilesetCreationDialogEvent final {};

struct InspectTilesetEvent final {};

struct LoadTilesetEvent final {
  fs::path path;
  Int2 tile_size {};
};

struct RemoveTilesetEvent final {
  UUID tileset_id {};
};

struct SelectTilesetEvent final {
  UUID tileset_id {};
};

struct SetTilesetSelectionEvent final {
  Region selection;
};

/// Event for changing the name a tileset.
struct RenameTilesetEvent final {
  UUID tileset_id {};  /// Target tileset.
  std::string name;    /// New tileset name.
};

}  // namespace tactile
