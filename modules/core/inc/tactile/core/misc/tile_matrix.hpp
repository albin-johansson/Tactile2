// Copyright (C) 2023 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <compare>  // strong_ordering

#include "tactile/core/api.hpp"
#include "tactile/core/container/vector.hpp"
#include "tactile/core/prelude.hpp"

namespace tactile {

/**
 * \brief Represents a single row of tile identifiers.
 */
using TileRow = Vector<TileID>;

/**
 * \brief Represents a two-dimensional grid of tile identifiers.
 */
using TileMatrix = Vector<TileRow>;

/**
 * \brief Represents the dimensions of a tile matrix.
 */
struct MatrixExtent final {
  usize row_count {};  ///< The number of tile rows.
  usize col_count {};  ///< The number of tile columns.

  [[nodiscard]] constexpr auto operator<=>(const MatrixExtent&) const noexcept
      -> std::strong_ordering = default;
};

/**
 * \brief Represents a position in a tile matrix.
 */
struct MatrixIndex final {
  usize row {};  ///< The tile row (Y-axis) index.
  usize col {};  ///< The tile column (X-axis) index.

  [[nodiscard]] constexpr auto operator<=>(const MatrixIndex&) const noexcept
      -> std::strong_ordering = default;
};

/**
 * \brief Creates an empty tile row.
 *
 * \param col_count the number of tile columns.
 *
 * \return a tile row.
 */
[[nodiscard]]
TACTILE_CORE_API auto make_tile_row(usize col_count) -> TileRow;

/**
 * \brief Creates an empty tile matrix.
 *
 * \param extent the size of the matrix.
 *
 * \return a tile matrix.
 */
[[nodiscard]]
TACTILE_CORE_API auto make_tile_matrix(const MatrixExtent& extent) -> TileMatrix;

/**
 * \brief Converts a one-dimensional tile index to a corresponding two-dimensional index.
 *
 * \details This function is particularly useful when dealing with tile identifier
 *          streams, usually from save files, where it's necessary to convert linear tile
 *          indices to more intuitive matrix coordinates.
 *
 * \param index     the linear tile index.
 * \param col_count the number of tile columns.
 *
 * \return a matrix index.
 */
[[nodiscard]]
constexpr auto to_matrix_index(const usize index, const usize col_count) noexcept
    -> MatrixIndex
{
  return MatrixIndex {.row = index / col_count, .col = index % col_count};
}

}  // namespace tactile