#include "core/utils/formatted_string.hpp"

#include <filesystem>  // path

#include <gtest/gtest.h>

using namespace tactile;

TEST(FormattedString, NoFormatting)
{
  const formatted_string str{"foobar"};

  ASSERT_STREQ("foobar", str.data());
  ASSERT_EQ("foobar", str.view());

  ASSERT_EQ(6u, str.size());
  ASSERT_EQ(128u, str.capacity());
}

TEST(FormattedString, SimpleFormatting)
{
  const formatted_string str{"Hello, {}!", "World"};
  ASSERT_EQ("Hello, World!", str.view());
}

TEST(FormattedString, AdvancedFormatting)
{
  const std::filesystem::path path = "foo/bar.txt";
  const formatted_string str{"The answer is {}, here's a path: {}", 42, path};
  ASSERT_EQ("The answer is 42, here's a path: \"foo/bar.txt\"", str.view());
}

TEST(FormattedString, SpareCapacity)
{
  const formatted_string<16> str{"123"};

  ASSERT_EQ("123", str.view());
  ASSERT_STREQ("123", str.data());

  ASSERT_EQ(3u, str.size());
  ASSERT_EQ(16u, str.capacity());
}

TEST(FormattedString, ExactCapacity)
{
  const formatted_string<5> str{"12345"};

  ASSERT_EQ("12345", str.view());
  ASSERT_STREQ("12345", str.data());

  ASSERT_EQ(5u, str.size());
  ASSERT_EQ(5u, str.capacity());
}

TEST(FormattedString, NotEnoughCapacity)
{
  const formatted_string<4> str{"12345"};

  ASSERT_EQ("1234", str.view());
  ASSERT_STREQ("1234", str.data());

  ASSERT_EQ(4u, str.size());
  ASSERT_EQ(4u, str.capacity());
}

TEST(FormattedString, FormatArgumentCapacityOverflow)
{
  const formatted_string<6> str{"1234{}", "567"};

  ASSERT_EQ("123456", str.view());
  ASSERT_STREQ("123456", str.data());

  ASSERT_EQ(6u, str.size());
  ASSERT_EQ(6u, str.capacity());
}