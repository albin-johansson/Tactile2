// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/io/int_parser.hpp"

#include <cstdint>      // int64_t, uint64_t
#include <string_view>  // string_view

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::parse [integral]
TEST(StringConv, ParseInvalidInt)
{
  EXPECT_FALSE(parse<std::int64_t>("").has_value());
  EXPECT_FALSE(parse<std::int64_t>(" ").has_value());
  EXPECT_FALSE(parse<std::int64_t>("42 ").has_value());
  EXPECT_FALSE(parse<std::int64_t>("foo42").has_value());
  EXPECT_FALSE(parse<std::int64_t>("42foo").has_value());
  EXPECT_FALSE(parse<std::int64_t>("+42").has_value());
  EXPECT_FALSE(parse<std::int64_t>("A").has_value());
  EXPECT_FALSE(parse<std::int64_t>("2", 2).has_value());
  EXPECT_FALSE(parse<std::int64_t>("9", 8).has_value());
  EXPECT_FALSE(parse<std::int64_t>("G", 16).has_value());
  EXPECT_FALSE(parse<std::int64_t>("0xFF", 16).has_value());
}

// tactile::parse [integral]
TEST(StringConv, ParseValidInt)
{
  EXPECT_EQ(parse<std::int64_t>("0"), std::int64_t {0});
  EXPECT_EQ(parse<std::int64_t>("-182"), std::int64_t {-182});
  EXPECT_EQ(parse<std::int64_t>("99"), std::int64_t {99});
  EXPECT_EQ(parse<std::int64_t>("027"), std::int64_t {27});
  EXPECT_EQ(parse<std::int64_t>("1000", 2), std::int64_t {0b1000});
  EXPECT_EQ(parse<std::int64_t>("A", 16), std::int64_t {0xA});
  EXPECT_EQ(parse<std::int64_t>("F", 16), std::int64_t {0xF});
  EXPECT_EQ(parse<std::int64_t>("10", 8), std::int64_t {010});
  EXPECT_EQ(parse<std::int64_t>("7FFFFFFFFFFFFFFF", 16),
            std::numeric_limits<std::int64_t>::max());
  EXPECT_EQ(parse<std::int64_t>(std::string_view {"1234"}.substr(0, 2)), std::int64_t {12});
  EXPECT_EQ(parse<std::int64_t>(std::string_view {"1234"}.substr(2, 2)), std::int64_t {34});
}

// tactile::parse [integral]
TEST(StringConv, ParseInvalidUint)
{
  EXPECT_FALSE(parse<std::uint64_t>("").has_value());
  EXPECT_FALSE(parse<std::uint64_t>(" ").has_value());
  EXPECT_FALSE(parse<std::uint64_t>("42 ").has_value());
  EXPECT_FALSE(parse<std::uint64_t>("foo42").has_value());
  EXPECT_FALSE(parse<std::uint64_t>("42foo").has_value());
  EXPECT_FALSE(parse<std::uint64_t>("-92").has_value());
  EXPECT_FALSE(parse<std::uint64_t>("+42").has_value());
  EXPECT_FALSE(parse<std::uint64_t>("A").has_value());
  EXPECT_FALSE(parse<std::uint64_t>("2", 2).has_value());
  EXPECT_FALSE(parse<std::uint64_t>("9", 8).has_value());
  EXPECT_FALSE(parse<std::uint64_t>("G", 16).has_value());
  EXPECT_FALSE(parse<std::uint64_t>("0xFF", 16).has_value());
}

// tactile::parse [integral]
TEST(StringConv, ParseValidUint)
{
  EXPECT_EQ(parse<std::uint64_t>("42"), std::uint64_t {42});
  EXPECT_EQ(parse<std::uint64_t>("042"), std::uint64_t {42});
  EXPECT_EQ(parse<std::uint64_t>("111", 2), std::uint64_t {0b111});
  EXPECT_EQ(parse<std::uint64_t>("A", 16), std::uint64_t {0xA});
  EXPECT_EQ(parse<std::uint64_t>("F", 16), std::uint64_t {0xF});
  EXPECT_EQ(parse<std::uint64_t>("10", 8), std::uint64_t {010});
  EXPECT_EQ(parse<std::uint64_t>("FFFFFFFFFFFFFFFF", 16),
            std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(parse<std::uint64_t>(std::string_view {"1234"}.substr(0, 2)), std::uint64_t {12});
  EXPECT_EQ(parse<std::uint64_t>(std::string_view {"1234"}.substr(2, 2)), std::uint64_t {34});
}

}  // namespace
}  // namespace tactile
