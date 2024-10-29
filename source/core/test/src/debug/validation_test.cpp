// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/debug/validation.hpp"

#include <stdexcept>  // invalid_argument

#include <gtest/gtest.h>

namespace tactile::core {
namespace {

// tactile::core::require_not_null
TEST(Validation, RequireNotNullWithValidPointer)
{
  constexpr int value = 42;
  EXPECT_EQ(require_not_null(&value), &value);
}

// tactile::core::require_not_null
TEST(Validation, RequireNotNullWithNullPointer)
{
  EXPECT_THROW((void) require_not_null(nullptr), std::invalid_argument);
}

}  // namespace
}  // namespace tactile::core
