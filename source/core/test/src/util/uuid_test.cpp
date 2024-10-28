// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/util/uuid.hpp"

#include <algorithm>    // count, count_if
#include <cctype>       // isxdigit
#include <map>          // map
#include <type_traits>  // ...

#include <gtest/gtest.h>

namespace tactile::core {
namespace {

// tactile::core::UUID::UUID
// tactile::core::UUID::is_null
// tactile::core::UUID::hash_code
TEST(UUID, Defaults)
{
  constexpr UUID uuid {};

  EXPECT_TRUE(uuid.is_null());
  EXPECT_EQ(uuid, uuid);
  EXPECT_NE(uuid.hash_code(), 0u);

  EXPECT_TRUE(std::is_copy_constructible_v<UUID>);
  EXPECT_TRUE(std::is_move_constructible_v<UUID>);
  EXPECT_TRUE(std::is_copy_assignable_v<UUID>);
  EXPECT_TRUE(std::is_move_assignable_v<UUID>);
}

// tactile::core::UUID::generate
TEST(UUID, Generate)
{
  const auto a = UUID::generate();
  const auto b = UUID::generate();
  const auto c = UUID::generate();

  EXPECT_EQ(a, a);
  EXPECT_EQ(b, b);
  EXPECT_EQ(c, c);

  EXPECT_NE(a, b);
  EXPECT_NE(a, c);
  EXPECT_NE(b, c);

  EXPECT_FALSE(a.is_null());
  EXPECT_FALSE(b.is_null());
  EXPECT_FALSE(c.is_null());
}

// tactile:UUID::generate
// tactile:UUID::hash_code
TEST(UUID, HashCode)
{
  const auto a = UUID::generate();
  const auto b = UUID::generate();
  EXPECT_EQ(a.hash_code(), a.hash_code());
  EXPECT_EQ(b.hash_code(), b.hash_code());
  EXPECT_NE(a.hash_code(), b.hash_code());
}

// tactile::core::to_string(const UUID&)
TEST(UUID, ToString)
{
  auto is_hex_digit = [](const char digit) -> bool {
    return std::isxdigit(static_cast<unsigned char>(digit));
  };

  {
    const UUID null {};
    const auto str = to_string(null);
    EXPECT_EQ(to_string(null), "00000000-0000-0000-0000-000000000000");
  }

  {
    const auto uuid = UUID::generate();
    const auto str = to_string(uuid);
    EXPECT_EQ(std::ranges::count(str, '-'), 4u);
    EXPECT_EQ(std::ranges::count_if(str, is_hex_digit), 32u);
    ASSERT_EQ(str.size(), 36u);
    EXPECT_EQ(str[8], '-');
    EXPECT_EQ(str[13], '-');
    EXPECT_EQ(str[18], '-');
    EXPECT_EQ(str[23], '-');
    EXPECT_NE(str, "00000000-0000-0000-0000-000000000000");
  }
}

// tactile::core::operator<(const UUID&, const UUID&)
TEST(UUID, LessThanOperator)
{
  const auto a = UUID::generate();
  const auto b = UUID::generate();

  EXPECT_NE(a < b, b < a);
  EXPECT_NO_THROW((void) (a < b));
  EXPECT_NO_THROW((void) (b < a));

  std::map<UUID, int> map {};
  EXPECT_EQ(map.size(), 0);
  EXPECT_FALSE(map.contains(a));
  EXPECT_FALSE(map.contains(b));

  map[a] = 10;
  EXPECT_EQ(map.size(), 1);
  EXPECT_TRUE(map.contains(a));
  EXPECT_FALSE(map.contains(b));

  map[b] = 20;
  EXPECT_EQ(map.size(), 2);
  EXPECT_TRUE(map.contains(a));
  EXPECT_TRUE(map.contains(b));
}

}  // namespace
}  // namespace tactile::core
