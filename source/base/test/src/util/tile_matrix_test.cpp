// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/util/tile_matrix.hpp"

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::make_tile_matrix
TEST(TileMatrix, MakeTileMatrix)
{
  constexpr Extent2D extent {3, 4};
  const auto tile_matrix = make_tile_matrix(extent);

  ASSERT_EQ(tile_matrix.size(), extent.rows);
  EXPECT_EQ(tile_matrix.at(0).size(), extent.cols);
  EXPECT_EQ(tile_matrix.at(1).size(), extent.cols);
  EXPECT_EQ(tile_matrix.at(2).size(), extent.cols);

  for (Extent2D::value_type row = 0; row < extent.rows; ++row) {
    for (Extent2D::value_type col = 0; col < extent.cols; ++col) {
      EXPECT_EQ(tile_matrix.at(row).at(col), kEmptyTile);
    }
  }
}

}  // namespace
}  // namespace tactile
