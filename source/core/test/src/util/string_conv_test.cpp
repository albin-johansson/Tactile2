// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/util/string_conv.hpp"

#include <limits>  // numeric_limits

#include <gtest/gtest.h>

namespace tactile::core {
namespace {

// tactile::core::to_native_string
TEST(StringConv, ToNativeString)
{
  EXPECT_EQ(to_native_string(""), TACTILE_NATIVE_STR(""));
  EXPECT_EQ(to_native_string("1"), TACTILE_NATIVE_STR("1"));
  EXPECT_EQ(to_native_string("foo"), TACTILE_NATIVE_STR("foo"));
  EXPECT_EQ(to_native_string("foo\0"), TACTILE_NATIVE_STR("foo\0"));
  EXPECT_EQ(to_native_string("bar.txt"), TACTILE_NATIVE_STR("bar.txt"));
  EXPECT_EQ(to_native_string("foo/bar"), TACTILE_NATIVE_STR("foo/bar"));
  EXPECT_EQ(to_native_string("foo/bar.txt"), TACTILE_NATIVE_STR("foo/bar.txt"));
  EXPECT_EQ(to_native_string("\0"), TACTILE_NATIVE_STR("\0"));

  const std::string_view foo {"foobar", 3};
  EXPECT_EQ(to_native_string(foo), TACTILE_NATIVE_STR("foo"));
}

// tactile::core::from_native_string
TEST(StringConv, FromNativeString)
{
  EXPECT_EQ(from_native_string(TACTILE_NATIVE_STR("")), "");
  EXPECT_EQ(from_native_string(TACTILE_NATIVE_STR("1")), "1");
  EXPECT_EQ(from_native_string(TACTILE_NATIVE_STR("foo")), "foo");
  EXPECT_EQ(from_native_string(TACTILE_NATIVE_STR("foo\0")), "foo");
  EXPECT_EQ(from_native_string(TACTILE_NATIVE_STR("bar.txt")), "bar.txt");
  EXPECT_EQ(from_native_string(TACTILE_NATIVE_STR("foo/bar")), "foo/bar");
  EXPECT_EQ(from_native_string(TACTILE_NATIVE_STR("foo/bar.txt")), "foo/bar.txt");
  EXPECT_EQ(from_native_string(TACTILE_NATIVE_STR("\0")), "\0");

  const NativeStringView foo {TACTILE_NATIVE_STR("foobar"), 3};
  EXPECT_EQ(from_native_string(foo), "foo");
}

// tactile::core::parse_float
TEST(StringConv, ParseFloat)
{
  // Invalid inputs
  EXPECT_FALSE(parse_float("").has_value());
  EXPECT_FALSE(parse_float(" ").has_value());
  EXPECT_FALSE(parse_float(".").has_value());
  EXPECT_FALSE(parse_float("4.2 ").has_value());
  EXPECT_FALSE(parse_float("foo9.0").has_value());
  EXPECT_FALSE(parse_float("40.92foo").has_value());
  EXPECT_FALSE(parse_float("+4.5").has_value());

  // Valid inputs
  EXPECT_EQ(parse_float("0"), 0.0);
  EXPECT_EQ(parse_float("42"), 42.0);
  EXPECT_EQ(parse_float("5."), 5.0);
  EXPECT_EQ(parse_float(".1"), 0.1);
  EXPECT_EQ(parse_float("1.0"), 1.0);
  EXPECT_EQ(parse_float("3.2"), 3.2);
  EXPECT_EQ(parse_float("-1.0"), -1.0);
  EXPECT_EQ(parse_float(std::string_view {"1234"}.substr(0, 2)), 12.0);
  EXPECT_EQ(parse_float(std::string_view {"1234"}.substr(2, 2)), 34.0);
}

}  // namespace
}  // namespace tactile::core
