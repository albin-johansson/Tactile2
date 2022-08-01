#include "core/util/fmt_string.hpp"

#include <filesystem>  // path

#include <gtest/gtest.h>

namespace tactile::test {

TEST(FmtString, NoFormatting)
{
  const FmtString str {"foobar"};

  ASSERT_STREQ("foobar", str.data());
  ASSERT_EQ("foobar", str.view());

  ASSERT_EQ(6u, str.size());
  ASSERT_EQ(128u, str.capacity());
}

TEST(FmtString, SimpleFormatting)
{
  const FmtString str {"Hello, {}!", "World"};
  ASSERT_EQ("Hello, World!", str.view());
}

TEST(FmtString, AdvancedFormatting)
{
  const std::filesystem::path path = "foo/bar.txt";
  const FmtString             str {"The answer is {}, here's a path: {}", 42, path};
  ASSERT_EQ("The answer is 42, here's a path: \"foo/bar.txt\"", str.view());
}

TEST(FmtString, SpareCapacity)
{
  const FmtString<16> str {"123"};

  ASSERT_EQ("123", str.view());
  ASSERT_STREQ("123", str.data());

  ASSERT_EQ(3u, str.size());
  ASSERT_EQ(16u, str.capacity());
}

TEST(FmtString, ExactCapacity)
{
  const FmtString<5> str {"12345"};

  ASSERT_EQ("12345", str.view());
  ASSERT_STREQ("12345", str.data());

  ASSERT_EQ(5u, str.size());
  ASSERT_EQ(5u, str.capacity());
}

TEST(FmtString, NotEnoughCapacity)
{
  const FmtString<4> str {"12345"};

  ASSERT_EQ("1234", str.view());
  ASSERT_STREQ("1234", str.data());

  ASSERT_EQ(4u, str.size());
  ASSERT_EQ(4u, str.capacity());
}

TEST(FmtString, FormatArgumentCapacityOverflow)
{
  const FmtString<6> str {"1234{}", "567"};

  ASSERT_EQ("123456", str.view());
  ASSERT_STREQ("123456", str.data());

  ASSERT_EQ(6u, str.size());
  ASSERT_EQ(6u, str.capacity());
}

}  // namespace tactile::test
