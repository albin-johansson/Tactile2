// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/numeric/offset_2d.hpp"

#include <gtest/gtest.h>

namespace tactile {
namespace {

TEST(Offset2D, Format)
{
  EXPECT_EQ(std::format("{}", Offset2D {-5, 7}), "(dx: -5, dy: 7)");
}

}  // namespace
}  // namespace tactile
