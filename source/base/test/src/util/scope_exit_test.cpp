// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/util/scope_exit.hpp"

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::ScopeExit::~ScopeExit
TEST(ScopeExit, Destructor)
{
  int value = 0;

  {
    const ScopeExit set_1 {[&] { value = 1; }};
    EXPECT_EQ(value, 0);

    {
      const ScopeExit set_2 {[&] { value = 2; }};
      EXPECT_EQ(value, 0);
    }

    EXPECT_EQ(value, 2);
  }

  EXPECT_EQ(value, 1);
}

// tactile::ScopeExit::ScopeExit
// tactile::ScopeExit::~ScopeExit
TEST(ScopeExit, ShouldCallFunctionJustOnce)
{
  int calls = 0;

  {
    ScopeExit increment {[&] { ++calls; }};
    ScopeExit increment2 {std::move(increment)};

    ScopeExit increment3 {};
    increment3 = std::move(increment2);
  }

  EXPECT_EQ(calls, 1);
}

}  // namespace
}  // namespace tactile
