// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/numeric/extent_2d.hpp"

#include <gtest/gtest.h>

namespace tactile {
namespace {

TEST(Extent2D, Format)
{
  EXPECT_EQ(std::format("{}", Extent2D {24, 59}), "(rows: 24, cols: 59)");
}

}  // namespace
}  // namespace tactile
