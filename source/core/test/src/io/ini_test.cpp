// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/io/ini.hpp"

#include <gtest/gtest.h>

namespace tactile::core {
namespace {

// tactile::core::parse_ini
TEST(Ini, ParseIni)
{
  const auto ini = parse_ini("assets/test/core/test.ini");

  ASSERT_TRUE(ini.has_value());
  ASSERT_EQ(ini->size(), 3);

  EXPECT_EQ(ini->at("s1").at("a"), "s1.a");
  EXPECT_EQ(ini->at("s1").at("b"), "s1.b");
  EXPECT_EQ(ini->at("s1").at("ABC"), "123");
  EXPECT_EQ(ini->at("s1").at("zxc"), "z x c");

  EXPECT_EQ(ini->at("s2").at("a"), "s2.a");
  EXPECT_EQ(ini->at("s2").at("b"), "s2.b");
  EXPECT_EQ(ini->at("s2").at("c"), "s2.c");

  EXPECT_TRUE(ini->at("s3").empty());
}

}  // namespace
}  // namespace tactile::core
