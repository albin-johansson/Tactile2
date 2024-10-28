// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/numeric/saturate_cast.hpp"

#include <cstdint>  // int8_t, uint8_t, int16_t, int32_t
#include <limits>   // numeric_limits

#include <gtest/gtest.h>

namespace tactile {
namespace {

template <typename T = std::int8_t>
inline constexpr T kMinI8 = T {std::numeric_limits<std::int8_t>::min()};

template <typename T = std::int8_t>
inline constexpr T kMaxI8 = T {std::numeric_limits<std::int8_t>::max()};

template <typename T = std::uint8_t>
inline constexpr T kMaxU8 = T {std::numeric_limits<std::uint8_t>::max()};

template <typename T = std::int16_t>
inline constexpr T kMinI16 = T {std::numeric_limits<std::int16_t>::min()};

template <typename T = std::int16_t>
inline constexpr T kMaxI16 = T {std::numeric_limits<std::int16_t>::max()};

// tactile::saturate_cast
TEST(SaturateCast, SignedToSignedNarrowing)
{
  EXPECT_EQ(saturate_cast<std::int8_t>(std::int16_t {0x00}), std::int8_t {0x00});
  EXPECT_EQ(saturate_cast<std::int8_t>(std::int16_t {0x41}), std::int8_t {0x41});
  EXPECT_EQ(saturate_cast<std::int8_t>(std::int16_t {-0x2E}), std::int8_t {-0x2E});

  EXPECT_EQ(saturate_cast<std::int8_t>(kMinI8<std::int16_t>), kMinI8<>);
  EXPECT_EQ(saturate_cast<std::int8_t>(kMinI8<std::int16_t> - std::int16_t {1}), kMinI8<>);

  EXPECT_EQ(saturate_cast<std::int8_t>(kMaxI8<std::int16_t>), kMaxI8<>);
  EXPECT_EQ(saturate_cast<std::int8_t>(kMaxI8<std::int16_t> + std::int16_t {1}), kMaxI8<>);
}

// tactile::saturate_cast
TEST(SaturateCast, SignedToSignedPromoting)
{
  EXPECT_EQ(saturate_cast<std::int32_t>(std::int16_t {0}), std::int32_t {0});
  EXPECT_EQ(saturate_cast<std::int32_t>(std::int16_t {12'345}), 12'345);

  EXPECT_EQ(saturate_cast<std::int32_t>(kMinI16<>), kMinI16<std::int32_t>);
  EXPECT_EQ(saturate_cast<std::int32_t>(kMaxI16<>), kMaxI16<std::int32_t>);
}

// tactile::saturate_cast
TEST(SaturateCast, UnsignedToSignedNarrowing)
{
  EXPECT_EQ(saturate_cast<std::int8_t>(std::uint8_t {0}), std::int8_t {0});
  EXPECT_EQ(saturate_cast<std::int8_t>(std::uint8_t {42}), std::int8_t {42});

  EXPECT_EQ(saturate_cast<std::int8_t>(kMaxI8<std::uint8_t>), kMaxI8<>);
  EXPECT_EQ(saturate_cast<std::int8_t>(kMaxI8<std::uint8_t> + std::uint8_t {1}), kMaxI8<>);
}

// tactile::saturate_cast
TEST(SaturateCast, UnsignedToSignedPromoting)
{
  EXPECT_EQ(saturate_cast<std::int16_t>(std::uint8_t {0}), std::int16_t {0});
  EXPECT_EQ(saturate_cast<std::int16_t>(std::uint8_t {0xEA}), std::int16_t {0xEA});

  EXPECT_EQ(saturate_cast<std::int16_t>(kMaxU8<>), kMaxU8<std::int16_t>);
}

// tactile::saturate_cast
TEST(SaturateCast, SignedToUnsignedNarrowing)
{
  EXPECT_EQ(saturate_cast<std::uint8_t>(std::int16_t {0}), std::uint8_t {0});
  EXPECT_EQ(saturate_cast<std::uint8_t>(std::int16_t {-12}), std::uint8_t {0});
  EXPECT_EQ(saturate_cast<std::uint8_t>(std::int16_t {0x4E}), std::uint8_t {0x4E});

  EXPECT_EQ(saturate_cast<std::uint8_t>(kMinI16<>), std::uint8_t {0});
  EXPECT_EQ(saturate_cast<std::uint8_t>(kMaxI16<>), kMaxU8<>);

  EXPECT_EQ(saturate_cast<std::uint8_t>(kMaxU8<std::int16_t>), kMaxU8<>);
  EXPECT_EQ(saturate_cast<std::uint8_t>(kMaxU8<std::int16_t> + std::int16_t {1}), kMaxU8<>);
}

// tactile::saturate_cast
TEST(SaturateCast, SignedToUnsignedPromoting)
{
  EXPECT_EQ(saturate_cast<std::uint8_t>(std::int8_t {0}), std::uint8_t {0});
  EXPECT_EQ(saturate_cast<std::uint8_t>(std::int8_t {-1}), std::uint8_t {0});
  EXPECT_EQ(saturate_cast<std::uint8_t>(std::int8_t {99}), std::uint8_t {99});

  EXPECT_EQ(saturate_cast<std::uint8_t>(kMinI8<>), std::uint8_t {0});
  EXPECT_EQ(saturate_cast<std::uint8_t>(kMaxI8<>), kMaxI8<std::uint8_t>);
}

}  // namespace
}  // namespace tactile
