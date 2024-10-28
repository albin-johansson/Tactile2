// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/numeric/vec_stream.hpp"

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::operator<< [Vec]
TEST(VecStream, StreamOperator)
{
  const Int4 vec {1, 2, 3, 4};

  std::stringstream stream {};
  stream << vec;

  EXPECT_EQ(stream.str(), "{1, 2, 3, 4}");
}

}  // namespace
}  // namespace tactile
