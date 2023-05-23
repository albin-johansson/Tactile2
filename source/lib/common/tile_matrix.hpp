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

#include <concepts>  // integral

#include "common/primitives.hpp"
#include "common/tile_extent.hpp"
#include "common/type/vec.hpp"

namespace tactile {

using TileRow = Vec<TileID>;
using TileMatrix = Vec<TileRow>;

template <typename T>
struct MatrixCoords final {
  T row {};
  T col {};
};

/// Converts a matrix index to the corresponding row/column pair.
///
/// This function is useful when parsing tile streams from save files, where you are
/// likely to want to convert the indices to more intuitive matrix coordinates.
///
/// \param index the index of the cell to determine the coordinates of.
/// \param nColumns the total number of columns in the matrix.
///
/// \return a pair encoding the matrix coordinates as (row, column).
template <std::integral T>
[[nodiscard]] constexpr auto to_matrix_coords(const T index, const T n_columns) noexcept
    -> MatrixCoords<T>
{
  return {index / n_columns, index % n_columns};
}

[[nodiscard]] auto make_tile_row(usize column_count) -> TileRow;

/// Creates a tile matrix of empty tile identifiers.
[[nodiscard]] auto make_tile_matrix(TileExtent extent) -> TileMatrix;

}  // namespace tactile