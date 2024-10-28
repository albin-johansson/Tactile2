// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/numeric/vec_format.hpp"

#include <format>  // format

#include <gtest/gtest.h>

namespace tactile {
namespace {

TEST(VecFormat, Formatting)
{
  EXPECT_EQ(std::format("{}", Int2 {1, 2}), "{1, 2}");
  EXPECT_EQ(std::format("{}", Int3 {1, 2, 3}), "{1, 2, 3}");
  EXPECT_EQ(std::format("{}", Int4 {1, 2, 3, 4}), "{1, 2, 3, 4}");
}

}  // namespace
}  // namespace tactile
