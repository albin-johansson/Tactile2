// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/container/buffer.hpp"

#include <algorithm>  // fill_n
#include <array>      // array
#include <iterator>   // distance, back_inserter
#include <vector>     // vector

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace tactile {
namespace {

using testing::Const;

// tactile::Buffer::Buffer
TEST(Buffer, DefaultConstructor)
{
  Buffer buffer;  // NOLINT

  EXPECT_FALSE(buffer.full());
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_EQ(buffer.capacity(), kDefaultMemoryBufferCapacity);

  EXPECT_EQ(buffer.data(), nullptr);
  EXPECT_NE(buffer.begin(), nullptr);
  EXPECT_NE(buffer.end(), nullptr);

  EXPECT_EQ(Const(buffer).data(), nullptr);
  EXPECT_NE(Const(buffer).begin(), nullptr);
  EXPECT_NE(Const(buffer).end(), nullptr);

  EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), 0);
  EXPECT_EQ(std::distance(Const(buffer).begin(), Const(buffer).end()), 0);
}

// tactile::Buffer::clear
TEST(Buffer, Clear)
{
  Buffer<char> buffer;  // NOLINT
  ASSERT_EQ(buffer.size(), 0);

  buffer.push_back('1');
  buffer.push_back('2');
  buffer.push_back('3');
  EXPECT_FALSE(buffer.empty());
  EXPECT_EQ(buffer.size(), 3);

  buffer.clear();
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.size(), 0);
}

// tactile::Buffer::push_back
TEST(Buffer, PushBack)
{
  Buffer<char> buffer;  // NOLINT
  ASSERT_EQ(buffer.size(), 0);

  buffer.push_back('f');
  ASSERT_EQ(buffer.size(), 1);
  EXPECT_EQ(buffer.view()[0], 'f');

  buffer.push_back('o');
  ASSERT_EQ(buffer.size(), 2);
  EXPECT_EQ(buffer.view()[0], 'f');
  EXPECT_EQ(buffer.view()[1], 'o');

  buffer.push_back('o');
  ASSERT_EQ(buffer.size(), 3);
  EXPECT_EQ(buffer.view()[0], 'f');
  EXPECT_EQ(buffer.view()[1], 'o');
  EXPECT_EQ(buffer.view()[2], 'o');

  buffer.push_back('b');
  ASSERT_EQ(buffer.size(), 4);
  EXPECT_EQ(buffer.view()[0], 'f');
  EXPECT_EQ(buffer.view()[1], 'o');
  EXPECT_EQ(buffer.view()[2], 'o');
  EXPECT_EQ(buffer.view()[3], 'b');

  buffer.push_back('a');
  ASSERT_EQ(buffer.size(), 5);
  EXPECT_EQ(buffer.view()[0], 'f');
  EXPECT_EQ(buffer.view()[1], 'o');
  EXPECT_EQ(buffer.view()[2], 'o');
  EXPECT_EQ(buffer.view()[3], 'b');
  EXPECT_EQ(buffer.view()[4], 'a');

  buffer.push_back('r');
  ASSERT_EQ(buffer.size(), 6);
  EXPECT_EQ(buffer.view()[0], 'f');
  EXPECT_EQ(buffer.view()[1], 'o');
  EXPECT_EQ(buffer.view()[2], 'o');
  EXPECT_EQ(buffer.view()[3], 'b');
  EXPECT_EQ(buffer.view()[4], 'a');
  EXPECT_EQ(buffer.view()[5], 'r');
}

// tactile::Buffer::push_back
TEST(Buffer, PushBackWithOverflow)
{
  Buffer<char> buffer;  // NOLINT
  ASSERT_EQ(buffer.size(), 0);

  std::fill_n(std::back_inserter(buffer), buffer.capacity(), '?');
  ASSERT_EQ(buffer.size(), buffer.capacity());
  EXPECT_TRUE(buffer.full());
}

// tactile::Buffer::push_back
TEST(Buffer, BackInserterCompatibility)
{
  Buffer<char> buffer;  // NOLINT
  ASSERT_EQ(buffer.size(), 0);

  std::fill_n(std::back_inserter(buffer), buffer.capacity(), '?');

  EXPECT_TRUE(buffer.full());
  ASSERT_EQ(buffer.size(), buffer.capacity());
  EXPECT_EQ(buffer.view().front(), '?');
  EXPECT_EQ(buffer.view().back(), '?');

  // This should not affect the buffer, it's already full.
  std::fill_n(std::back_inserter(buffer), 10, '!');

  EXPECT_TRUE(buffer.full());
  ASSERT_EQ(buffer.size(), buffer.capacity());
  EXPECT_EQ(buffer.view().front(), '?');
  EXPECT_EQ(buffer.view().back(), '?');
}

// tactile::Buffer::append [iterator version]
TEST(Buffer, AppendIteratorRange)
{
  const std::array<char, 4> values = {'!', '?', '+', '-'};
  Buffer<char, 7> buffer;  // NOLINT

  buffer.append(values.begin(), values.end());
  EXPECT_EQ(buffer.size(), 4);
  EXPECT_EQ(buffer.capacity() - buffer.size(), 3);
  EXPECT_FALSE(buffer.full());

  // All elements won't fit, only some will be included.
  buffer.append(values.begin(), values.end());
  EXPECT_EQ(buffer.size(), 7);
  EXPECT_EQ(buffer.capacity() - buffer.size(), 0);
  EXPECT_TRUE(buffer.full());

  EXPECT_EQ(buffer.view()[0], '!');
  EXPECT_EQ(buffer.view()[1], '?');
  EXPECT_EQ(buffer.view()[2], '+');
  EXPECT_EQ(buffer.view()[3], '-');
  EXPECT_EQ(buffer.view()[4], '!');
  EXPECT_EQ(buffer.view()[5], '?');
  EXPECT_EQ(buffer.view()[6], '+');
}

// tactile::Buffer::append [container version]
TEST(Buffer, AppendContainer)
{
  const std::vector<std::uint32_t> values = {1, 2, 3, 4, 5};
  Buffer<std::uint32_t, 8> buffer;  // NOLINT
  ASSERT_EQ(buffer.size(), 0);

  buffer.append(values);
  ASSERT_EQ(buffer.size(), 5);
  EXPECT_EQ(buffer.capacity(), 8);
  EXPECT_FALSE(buffer.full());

  EXPECT_EQ(buffer.view()[0], 1);
  EXPECT_EQ(buffer.view()[1], 2);
  EXPECT_EQ(buffer.view()[2], 3);
  EXPECT_EQ(buffer.view()[3], 4);
  EXPECT_EQ(buffer.view()[4], 5);

  buffer.append(values);
  ASSERT_EQ(buffer.size(), 8);
  EXPECT_EQ(buffer.capacity(), 8);
  EXPECT_TRUE(buffer.full());

  EXPECT_EQ(buffer.view()[0], 1);
  EXPECT_EQ(buffer.view()[1], 2);
  EXPECT_EQ(buffer.view()[2], 3);
  EXPECT_EQ(buffer.view()[3], 4);
  EXPECT_EQ(buffer.view()[4], 5);
  EXPECT_EQ(buffer.view()[5], 1);
  EXPECT_EQ(buffer.view()[6], 2);
  EXPECT_EQ(buffer.view()[7], 3);
}

// tactile::Buffer::set_terminator
TEST(Buffer, SetTerminatorWithRemainingCapacity)
{
  Buffer<char, 4> buffer {};
  buffer.push_back('A');
  buffer.push_back('B');

  ASSERT_EQ(buffer.size(), 2);
  EXPECT_EQ(buffer.view()[0], 'A');
  EXPECT_EQ(buffer.view()[1], 'B');

  buffer.set_terminator('\0');

  ASSERT_EQ(buffer.size(), 3);
  EXPECT_EQ(buffer.view()[0], 'A');
  EXPECT_EQ(buffer.view()[1], 'B');
  EXPECT_EQ(buffer.view()[2], '\0');
}

// tactile::Buffer::set_terminator
TEST(Buffer, SetTerminatorWithFullBuffer)
{
  Buffer<char, 4> buffer {};

  std::fill_n(std::back_inserter(buffer), buffer.capacity(), '?');

  ASSERT_EQ(buffer.size(), buffer.capacity());
  EXPECT_EQ(buffer.view()[0], '?');
  EXPECT_EQ(buffer.view()[1], '?');
  EXPECT_EQ(buffer.view()[2], '?');
  EXPECT_EQ(buffer.view()[3], '?');

  buffer.set_terminator('!');

  ASSERT_EQ(buffer.size(), buffer.capacity());
  EXPECT_EQ(buffer.view()[0], '?');
  EXPECT_EQ(buffer.view()[1], '?');
  EXPECT_EQ(buffer.view()[2], '?');
  EXPECT_EQ(buffer.view()[3], '!');
}

// tactile::Buffer::data
TEST(Buffer, Data)
{
  Buffer<char, 32> buffer;  // NOLINT
  ASSERT_EQ(buffer.size(), 0);

  EXPECT_EQ(buffer.data(), nullptr);
  EXPECT_EQ(Const(buffer).data(), nullptr);

  buffer.push_back('#');
  ASSERT_EQ(buffer.size(), 1);

  EXPECT_NE(buffer.data(), nullptr);
  EXPECT_NE(Const(buffer).data(), nullptr);
}

// tactile::Buffer::begin
// tactile::Buffer::end
TEST(Buffer, Iteration)
{
  Buffer<std::uint16_t, 6> buffer;  // NOLINT

  EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), 0);
  EXPECT_EQ(std::distance(Const(buffer).begin(), Const(buffer).end()), 0);

  buffer.push_back(0xF1);
  buffer.push_back(0xF2);
  buffer.push_back(0xF3);

  EXPECT_EQ(std::distance(buffer.begin(), buffer.end()), 3);
  EXPECT_EQ(std::distance(Const(buffer).begin(), Const(buffer).end()), 3);
}

// tactile::Buffer::view
TEST(Buffer, ViewWithCharBuffer)
{
  Buffer<char, 32> buffer;  // NOLINT

  buffer.push_back('f');
  buffer.push_back('o');
  buffer.push_back('o');
  buffer.push_back('b');
  buffer.push_back('a');
  buffer.push_back('r');

  EXPECT_EQ(buffer.size(), 6);
  EXPECT_EQ(buffer.view().size(), 6);
  EXPECT_EQ(buffer.view(), "foobar");
}

// tactile::Buffer::view
TEST(Buffer, ViewWithNonCharBuffer)
{
  Buffer<std::uint8_t, 16> buffer;  // NOLINT

  buffer.push_back(1u);
  buffer.push_back(2u);
  buffer.push_back(3u);

  EXPECT_EQ(buffer.size(), 3);
  ASSERT_EQ(buffer.view().size(), 3);
  EXPECT_EQ(buffer.view()[0], 1u);
  EXPECT_EQ(buffer.view()[1], 2u);
  EXPECT_EQ(buffer.view()[2], 3u);
}

}  // namespace
}  // namespace tactile
