// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/io/tile_io.hpp"

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::parse_raw_tile_matrix
TEST(TileIO, ParseRawTileMatrix)
{
  const ByteStream byte_stream {
    // clang-format off
    0x11, 0x22, 0x33, 0x44, // Tile 0
    0x11, 0x22, 0x33, 0x44, // Tile 1
    0x11, 0x22, 0x33, 0x44, // Tile 3
    0x11, 0x22, 0x33, 0x44, // Tile 4
    0x11, 0x22, 0x33, 0x44, // Tile 5
    0x11, 0x22, 0x33, 0x44, // Tile 6
    // clang-format on
  };

  constexpr Extent2D extent {.rows = 3, .cols = 2};
  const auto tile_matrix = parse_raw_tile_matrix(byte_stream, extent, TileIdFormat::kTactile);

  ASSERT_TRUE(tile_matrix.has_value());

  ASSERT_EQ(std::ssize(*tile_matrix), extent.rows);
  ASSERT_EQ(std::ssize(tile_matrix->at(0)), extent.cols);
  ASSERT_EQ(std::ssize(tile_matrix->at(1)), extent.cols);
  ASSERT_EQ(std::ssize(tile_matrix->at(2)), extent.cols);

  EXPECT_EQ(tile_matrix->at(0).at(0), TileID {0x44332211});
  EXPECT_EQ(tile_matrix->at(0).at(1), TileID {0x44332211});

  EXPECT_EQ(tile_matrix->at(1).at(0), TileID {0x44332211});
  EXPECT_EQ(tile_matrix->at(1).at(1), TileID {0x44332211});

  EXPECT_EQ(tile_matrix->at(2).at(0), TileID {0x44332211});
  EXPECT_EQ(tile_matrix->at(2).at(1), TileID {0x44332211});
}

// tactile::parse_raw_tile_matrix
TEST(TileIO, ParseRawTileMatrixWithInsufficientData)
{
  const ByteStream byte_stream {
    // clang-format off
    0x11, 0x22, 0x33, 0x44, // Tile 0
    0x11, 0x22, 0x33, 0x44, // Tile 1
    0x11, 0x22, 0x33, 0x44, // Tile 3
    0x11, 0x22, 0x33, 0x44, // Tile 4
    0x11, 0x22, 0x33, 0x44, // Tile 5
    0x11, 0x22, 0x33,       // Tile 6
    // clang-format on
  };

  constexpr Extent2D extent {.rows = 3, .cols = 2};
  const auto tile_matrix = parse_raw_tile_matrix(byte_stream, extent, TileIdFormat::kTactile);

  EXPECT_FALSE(tile_matrix.has_value());
}

// tactile::parse_raw_tile_matrix
TEST(TileIO, ParseRawTileMatrixWithTooMuchData)
{
  const ByteStream byte_stream {
    // clang-format off
    0x11, 0x22, 0x33, 0x44, // Tile 0
    0x11, 0x22, 0x33, 0x44, // Tile 1
    0x11, 0x22, 0x33, 0x44, // Tile 3
    0x11, 0x22, 0x33, 0x44, // Tile 4
    0x11, 0x22, 0x33, 0x44, // Tile 5
    0x11, 0x22, 0x33, 0x44, // Tile 6
    0x11,
    // clang-format on
  };

  constexpr Extent2D extent {.rows = 3, .cols = 2};
  const auto tile_matrix = parse_raw_tile_matrix(byte_stream, extent, TileIdFormat::kTactile);

  EXPECT_FALSE(tile_matrix.has_value());
}

// tactile::to_byte_stream [TileMatrix]
// tactile::parse_raw_tile_matrix
TEST(TileIO, TileMatrixToByteStreamAndBack)
{
  const TileMatrix original_tile_matrix {
    TileRow {10, 11, 12, 13},
    TileRow {20, 21, 22, 23},
    TileRow {30, 31, 32, 33},
  };

  const auto bytes = to_byte_stream(original_tile_matrix);
  EXPECT_EQ(bytes.size(), 12 * sizeof(TileID));

  const auto new_tile_matrix =
      parse_raw_tile_matrix(bytes, Extent2D {.rows = 3, .cols = 4}, TileIdFormat::kTactile);
  ASSERT_TRUE(new_tile_matrix.has_value());

  ASSERT_EQ(new_tile_matrix->size(), 3);
  ASSERT_EQ(new_tile_matrix->at(0).size(), 4);
  ASSERT_EQ(new_tile_matrix->at(1).size(), 4);
  ASSERT_EQ(new_tile_matrix->at(2).size(), 4);

  EXPECT_EQ(new_tile_matrix->at(0).at(0), TileID {10});
  EXPECT_EQ(new_tile_matrix->at(0).at(1), TileID {11});
  EXPECT_EQ(new_tile_matrix->at(0).at(2), TileID {12});
  EXPECT_EQ(new_tile_matrix->at(0).at(3), TileID {13});

  EXPECT_EQ(new_tile_matrix->at(1).at(0), TileID {20});
  EXPECT_EQ(new_tile_matrix->at(1).at(1), TileID {21});
  EXPECT_EQ(new_tile_matrix->at(1).at(2), TileID {22});
  EXPECT_EQ(new_tile_matrix->at(1).at(3), TileID {23});

  EXPECT_EQ(new_tile_matrix->at(2).at(0), TileID {30});
  EXPECT_EQ(new_tile_matrix->at(2).at(1), TileID {31});
  EXPECT_EQ(new_tile_matrix->at(2).at(2), TileID {32});
  EXPECT_EQ(new_tile_matrix->at(2).at(3), TileID {33});
}

}  // namespace
}  // namespace tactile
