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

#include "core/common/fs.hpp"
#include "core/common/ints.hpp"
#include "core/common/math.hpp"

namespace tactile {

/// Provides information necessary to construct tilesets.
struct TilesetInfo final
{
  fs::path texture_path;     /// Path to associated texture.
  uint     texture_id {};    /// The associated OpenGL texture identifier.
  int2     texture_size {};  /// The size of the texture.
  int2     tile_size {};     /// Logical tile size.
};

}  // namespace tactile
