// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/numeric/vec_common.hpp"

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::abs [Vec]
TEST(VecCommon, Abs)
{
  const Int4 a {-1, 0, -52, 923};
  const Int4 b = abs(a);

  EXPECT_EQ(b.x(), 1);
  EXPECT_EQ(b.y(), 0);
  EXPECT_EQ(b.z(), 52);
  EXPECT_EQ(b.w(), 923);
}

// tactile::round [Vec]
TEST(Vec, Round)
{
  const Float4 a {1.1f, 32.9f, 4.5f, 0.7f};
  const Float4 b = round(a);

  EXPECT_EQ(b.x(), 1.0f);
  EXPECT_EQ(b.y(), 33.0f);
  EXPECT_EQ(b.z(), 5.0f);
  EXPECT_EQ(b.w(), 1.0f);
}

// tactile::floor [Vec]
TEST(Vec, Floor)
{
  const Float4 a {4.9f, 23.1f, 0.6f, 1.5f};
  const Float4 b = floor(a);

  EXPECT_EQ(b.x(), 4.0f);
  EXPECT_EQ(b.y(), 23.0f);
  EXPECT_EQ(b.z(), 0.0f);
  EXPECT_EQ(b.w(), 1.0f);
}

// tactile::ceil [Vec]
TEST(Vec, Ceil)
{
  const Float4 a {0.1f, -0.1f, 1.4f, 1.5f};
  const Float4 b = ceil(a);

  EXPECT_EQ(b.x(), 1.0f);
  EXPECT_EQ(b.y(), 0.0f);
  EXPECT_EQ(b.z(), 2.0f);
  EXPECT_EQ(b.w(), 2.0f);
}

// tactile::min [Vec]
TEST(Vec, Min)
{
  const Int4 a {-1, +2, -3, +4};
  const Int4 b {+1, -2, +3, -4};
  const auto ab_min = min(a, b);

  EXPECT_EQ(ab_min.x(), -1);
  EXPECT_EQ(ab_min.y(), -2);
  EXPECT_EQ(ab_min.z(), -3);
  EXPECT_EQ(ab_min.w(), -4);
}

// tactile::max [Vec]
TEST(Vec, Max)
{
  const Int4 a {-1, +2, -3, +4};
  const Int4 b {+1, -2, +3, -4};
  const auto ab_max = max(a, b);

  EXPECT_EQ(ab_max.x(), 1);
  EXPECT_EQ(ab_max.y(), 2);
  EXPECT_EQ(ab_max.z(), 3);
  EXPECT_EQ(ab_max.w(), 4);
}

// tactile::compare_elements
TEST(Vec, CompareElements)
{
  {
    const Int4 vec {1, 2, 3, 4};
    const auto result = compare_elements(vec, vec);
    EXPECT_TRUE(result.x());
    EXPECT_TRUE(result.y());
    EXPECT_TRUE(result.z());
    EXPECT_TRUE(result.w());
  }

  {
    const Int4 a {1, 2, 3, 4};
    const Int4 b {1, 5, 6, 7};
    const auto result = compare_elements(a, b);
    EXPECT_TRUE(result.x());
    EXPECT_FALSE(result.y());
    EXPECT_FALSE(result.z());
    EXPECT_FALSE(result.w());
  }

  {
    const Int4 a {2, 1, 4, 6};
    const Int4 b {3, 1, 5, 7};
    const auto result = compare_elements(a, b);
    EXPECT_FALSE(result.x());
    EXPECT_TRUE(result.y());
    EXPECT_FALSE(result.z());
    EXPECT_FALSE(result.w());
  }

  {
    const Int4 a {2, 4, 1, 6};
    const Int4 b {3, 5, 1, 7};
    const auto result = compare_elements(a, b);
    EXPECT_FALSE(result.x());
    EXPECT_FALSE(result.y());
    EXPECT_TRUE(result.z());
    EXPECT_FALSE(result.w());
  }

  {
    const Int4 a {2, 4, 6, 1};
    const Int4 b {3, 5, 7, 1};
    const auto result = compare_elements(a, b);
    EXPECT_FALSE(result.x());
    EXPECT_FALSE(result.y());
    EXPECT_FALSE(result.z());
    EXPECT_TRUE(result.w());
  }

  {
    const Int4 a {1, 2, 3, 4};
    const Int4 b {5, 6, 7, 8};
    const auto result = compare_elements(a, b);
    EXPECT_FALSE(result.x());
    EXPECT_FALSE(result.y());
    EXPECT_FALSE(result.z());
    EXPECT_FALSE(result.w());
  }
}

}  // namespace
}  // namespace tactile
