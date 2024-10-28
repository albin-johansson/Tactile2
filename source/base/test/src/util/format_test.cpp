// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/util/format.hpp"

#include <cstddef>  // size_t

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::format_to_buffer
TEST(Format, FormatOneCharToBuffer)
{
  Buffer<char, 1> buffer;  // NOLINT
  format_to_buffer(buffer, "{}", "!");

  ASSERT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.view()[0], '!');
}

// tactile::format_to_buffer
TEST(Format, FormatToBuffer)
{
  Buffer<char, 32> buffer;  // NOLINT
  format_to_buffer(buffer, "{}", "foobar");

  ASSERT_EQ(buffer.size(), 6);
  EXPECT_EQ(buffer.view()[0], 'f');
  EXPECT_EQ(buffer.view()[1], 'o');
  EXPECT_EQ(buffer.view()[2], 'o');
  EXPECT_EQ(buffer.view()[3], 'b');
  EXPECT_EQ(buffer.view()[4], 'a');
  EXPECT_EQ(buffer.view()[5], 'r');
}

// tactile::format_to_buffer
TEST(Format, FormatToBufferWhenFull)
{
  Buffer<char, 32> buffer;  // NOLINT
  for (std::size_t i = 0; i < buffer.capacity(); ++i) {
    buffer.push_back('?');
  }

  ASSERT_EQ(buffer.size(), buffer.capacity());
  EXPECT_TRUE(buffer.full());

  format_to_buffer(buffer, "{}", "foobar");

  ASSERT_EQ(buffer.size(), buffer.capacity());
  EXPECT_EQ(buffer.view()[buffer.size() - 3], '?');
  EXPECT_EQ(buffer.view()[buffer.size() - 2], '?');
  EXPECT_EQ(buffer.view()[buffer.size() - 1], '?');
}

// tactile::format_to_buffer
TEST(Format, FormatToBufferWithOverflow)
{
  Buffer<char, 32> buffer;  // NOLINT
  for (std::size_t i = 0; i < buffer.capacity() - 3; ++i) {
    buffer.push_back('?');
  }

  ASSERT_EQ(buffer.size(), buffer.capacity() - 3);
  EXPECT_FALSE(buffer.full());

  format_to_buffer(buffer, "{}", "foo");

  ASSERT_EQ(buffer.size(), buffer.capacity());
  EXPECT_TRUE(buffer.full());

  EXPECT_EQ(buffer.view()[buffer.size() - 4], '?');
  EXPECT_EQ(buffer.view()[buffer.size() - 3], 'f');
  EXPECT_EQ(buffer.view()[buffer.size() - 2], 'o');
  EXPECT_EQ(buffer.view()[buffer.size() - 1], 'o');
}

}  // namespace
}  // namespace tactile
