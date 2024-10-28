// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/container/lookup.hpp"

#include <functional>     // less
#include <map>            // map
#include <stdexcept>      // out_of_range
#include <string>         // string
#include <unordered_map>  // unordered_map

#include <gtest/gtest.h>

#include "tactile/base/container/string_map.hpp"

namespace tactile {
namespace {

// tactile::exists_in
// tactile::find_in
// tactile::lookup_in
TEST(Lookup, EmptyMap)
{
  const StringMap<int> hash_map {};
  const std::map<std::string, int, std::less<>> tree_map {};

  EXPECT_FALSE(exists_in(hash_map, "foo"));
  EXPECT_FALSE(exists_in(tree_map, "foo"));

  EXPECT_EQ(find_in(hash_map, "foo"), nullptr);
  EXPECT_EQ(find_in(tree_map, "foo"), nullptr);

  EXPECT_THROW((void) lookup_in(hash_map, "foo"), std::out_of_range);
  EXPECT_THROW((void) lookup_in(tree_map, "foo"), std::out_of_range);
}

// tactile::find_in
TEST(Lookup, FindIn)
{
  std::map<int, std::string> map {};
  map[0] = "0";
  map[1] = "1";

  const auto& const_map = map;

  EXPECT_EQ(find_in(map, 0), &map.at(0));
  EXPECT_EQ(find_in(map, 1), &map.at(1));
  EXPECT_EQ(find_in(map, 2), nullptr);
  EXPECT_EQ(find_in(const_map, 0), &const_map.at(0));
  EXPECT_EQ(find_in(const_map, 1), &const_map.at(1));
  EXPECT_EQ(find_in(const_map, 2), nullptr);
}

// tactile::lookup_in
TEST(Lookup, LookupIn)
{
  StringMap<std::string> map {};
  map["A"] = "1";
  map["B"] = "2";

  const auto& const_map = map;

  EXPECT_EQ(lookup_in(map, "A"), "1");
  EXPECT_EQ(lookup_in(map, "B"), "2");
  EXPECT_EQ(lookup_in(const_map, "A"), "1");
  EXPECT_EQ(lookup_in(const_map, "B"), "2");

  EXPECT_THROW((void) lookup_in(map, ""), std::out_of_range);
  EXPECT_THROW((void) lookup_in(map, "a"), std::out_of_range);
  EXPECT_THROW((void) lookup_in(const_map, ""), std::out_of_range);
  EXPECT_THROW((void) lookup_in(const_map, "a"), std::out_of_range);
}

// tactile::erase_from
TEST(Lookup, EraseFrom)
{
  StringMap<int> map {};
  map["A"] = 0xA;
  map["B"] = 0xB;

  EXPECT_EQ(map.size(), 2);

  erase_from(map, "A");
  erase_from(map, "A");

  EXPECT_EQ(map.size(), 1);

  erase_from(map, "B");
  erase_from(map, "B");

  EXPECT_EQ(map.size(), 0);
}

// tactile::take_from
TEST(Lookup, TakeFrom)
{
  StringMap<int> map {};
  map["A"] = 0xA;
  map["B"] = 0xB;

  EXPECT_EQ(take_from(map, "A"), 0xA);
  EXPECT_EQ(take_from(map, "A"), std::nullopt);

  EXPECT_EQ(take_from(map, "B"), 0xB);
  EXPECT_EQ(take_from(map, "B"), std::nullopt);

  EXPECT_EQ(map.size(), 0);
}

// tactile::exists_in
TEST(Lookup, ExistsIn)
{
  StringMap<int> map {};
  map["A"] = 10;
  map["B"] = 20;
  map["C"] = 30;

  ASSERT_EQ(map.size(), 3);
  EXPECT_TRUE(exists_in(map, "A"));
  EXPECT_TRUE(exists_in(map, "B"));
  EXPECT_TRUE(exists_in(map, "C"));

  EXPECT_FALSE(exists_in(map, ""));
  EXPECT_FALSE(exists_in(map, "A "));
  EXPECT_FALSE(exists_in(map, "a"));
  EXPECT_FALSE(exists_in(map, "b"));
  EXPECT_FALSE(exists_in(map, "c"));
}

}  // namespace
}  // namespace tactile
