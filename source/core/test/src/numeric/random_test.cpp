// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/numeric/random.hpp"

#include <algorithm>  // generate, count
#include <iostream>   // cout

#include <gtest/gtest.h>

#include "tactile/base/container/array.hpp"

using namespace tactile;

/** \trace tactile::get_random_int */
TEST(Random, GetRandomInt)
{
  EXPECT_EQ(get_random_int(0, 0), 0);
  EXPECT_EQ(get_random_int(1, 1), 1);
  EXPECT_EQ(get_random_int(-1, -1), -1);

  const auto value = get_random_int(0, 10);
  EXPECT_GE(value, 0);
  EXPECT_LE(value, 10);
}

/** \trace tactile::get_random_uint */
TEST(Random, GetRandomUInt)
{
  EXPECT_EQ(get_random_uint(0u, 0u), 0u);
  EXPECT_EQ(get_random_uint(1u, 1u), 1u);
  EXPECT_EQ(get_random_uint(~0u, ~0u), ~0u);

  const auto value = get_random_int(0u, 10u);
  EXPECT_GE(value, 0u);
  EXPECT_LE(value, 10u);
}

/** \trace tactile::get_random_float */
TEST(Random, GetRandomFloat)
{
  const auto value = get_random_float(-4.2f, 83.1f);
  EXPECT_GE(value, -4.2f);
  EXPECT_LE(value, 83.1f);
}

/** \trace tactile::get_random_float_normalized */
TEST(Random, GetRandomFloatNormalized)
{
  const auto value = get_random_float_normalized();
  EXPECT_GT(value, 0.0f);
  EXPECT_LT(value, 1.0f);
}

/** \trace tactile::get_random_bool */
TEST(Random, GetRandomBool)
{
  Array<bool, 2'000> values;
  std::ranges::generate(values, &get_random_bool);

  const auto true_count = std::ranges::count(values, true);
  const auto false_count = std::ssize(values) - true_count;
  std::cout << "Called get_random_bool " << values.size() << " times: ";
  std::cout << true_count << " true, " << false_count << " false\n";

  // This is far from perfect, but detects suspicious (unlikely) ratios.
  const auto ratio = static_cast<double>(true_count) / static_cast<double>(false_count);
  EXPECT_GT(ratio, 0.80);
  EXPECT_LT(ratio, 1.20);
}
