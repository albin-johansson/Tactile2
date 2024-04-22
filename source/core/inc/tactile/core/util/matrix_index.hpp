// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstddef>     // size_t
#include <functional>  // hash
#include <ostream>     // ostream

#include "tactile/base/int.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/util/hash.hpp"

namespace tactile {

/**
 * Represents a valid location in a two-dimensional matrix.
 */
struct MatrixIndex final
{
  /** The row index. */
  usize row;

  /** The column index. */
  usize col;

  [[nodiscard]] auto operator==(const MatrixIndex&) const noexcept
      -> bool = default;
};

/**
 * Outputs a matrix index to a stream.
 *
 * \param stream The output stream.
 * \param index  The matrix index to emit.
 *
 * \return
 *    The provided stream.
 */
auto operator<<(std::ostream& stream,
                const MatrixIndex& index) -> std::ostream&;

}  // namespace tactile

template <>
struct std::hash<tactile::MatrixIndex> final
{
  [[nodiscard]]
  auto operator()(const tactile::MatrixIndex& index) const noexcept
      -> std::size_t
  {
    return tactile::hash_combine(index.row, index.col);
  }
};