// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/numeric/index_2d.hpp"

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::Index2D::from_1d
TEST(Index2D, From1D)
{
  EXPECT_EQ(Index2D::from_1d(0, 1), (Index2D {0, 0}));
  EXPECT_EQ(Index2D::from_1d(1, 1), (Index2D {0, 1}));

  EXPECT_EQ(Index2D::from_1d(0, 4), (Index2D {0, 0}));
  EXPECT_EQ(Index2D::from_1d(1, 4), (Index2D {1, 0}));
  EXPECT_EQ(Index2D::from_1d(2, 4), (Index2D {2, 0}));
  EXPECT_EQ(Index2D::from_1d(3, 4), (Index2D {3, 0}));

  EXPECT_EQ(Index2D::from_1d(4, 4), (Index2D {0, 1}));
  EXPECT_EQ(Index2D::from_1d(5, 4), (Index2D {1, 1}));
  EXPECT_EQ(Index2D::from_1d(6, 4), (Index2D {2, 1}));
  EXPECT_EQ(Index2D::from_1d(7, 4), (Index2D {3, 1}));

  EXPECT_EQ(Index2D::from_1d(8, 4), (Index2D {0, 2}));
  EXPECT_EQ(Index2D::from_1d(9, 4), (Index2D {1, 2}));
  EXPECT_EQ(Index2D::from_1d(10, 4), (Index2D {2, 2}));
  EXPECT_EQ(Index2D::from_1d(11, 4), (Index2D {3, 2}));
}

TEST(Index2D, Format)
{
  EXPECT_EQ(std::format("{}", Index2D {1, 2}), "(x1, y2)");
}

}  // namespace
}  // namespace tactile
