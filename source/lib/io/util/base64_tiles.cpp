/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "base64_tiles.hpp"

#include <bit>       // endian
#include <concepts>  // same_as
#include <cstring>   // memcpy

#include <cppcodec/base64_rfc4648.hpp>

#include "common/debug/panic.hpp"
#include "common/util/bit.hpp"
#include "common/util/functional.hpp"
#include "core/tile/tile_matrix.hpp"
#include "io/compression.hpp"

using Base64 = cppcodec::base64_rfc4648;

namespace tactile::io {
namespace {

// Update documentation if the representation of TileID changes
static_assert(std::same_as<TileID, int32>);

[[nodiscard]] auto convert_tile_matrix_to_sequence(const TileMatrix& matrix,
                                                   const TileExtent extent) -> ByteStream
{
  ByteStream seq;
  seq.reserve(extent.rows * extent.cols * sizeof(TileID));

  invoke_mn(extent.rows, extent.cols, [&](const usize row, const usize col) {
    // Always store tile identifiers as little endian values
    const auto tile_id = to_little_endian(matrix[row][col]);
    each_byte(tile_id, [&](const uint8 byte) { seq.push_back(byte); });
  });

  return seq;
}

[[nodiscard]] auto restore_tiles(const auto& data, const TileExtent extent) -> TileMatrix
{
  auto matrix = make_tile_matrix(extent);

  const auto count = data.size() / sizeof(int32);
  for (usize i = 0; i < count; ++i) {
    const auto index = i * sizeof(int32);

    TileID tile {};
    std::memcpy(&tile, &data[index], sizeof(TileID));

    if constexpr (std::endian::native == std::endian::big) {
      tile = byteswap(tile);
    }

    const auto [row, col] = to_matrix_coords(i, extent.cols);
    matrix[row][col] = tile;
  }

  return matrix;
}

[[nodiscard]] auto encode_bytes(const ByteStream& stream) -> String
{
  return Base64::encode(stream.data(), stream.size());
}

}  // namespace

auto base64_encode_tiles(const TileMatrix& tiles,
                         const TileExtent extent,
                         const TileCompression compression) -> String
{
  const auto sequence = convert_tile_matrix_to_sequence(tiles, extent);

  switch (compression) {
    case TileCompression::None: {
      return encode_bytes(sequence);
    }
    case TileCompression::Zlib: {
      const auto compressed = zlib_compress(sequence).value();
      return encode_bytes(compressed);
    }
    case TileCompression::Zstd: {
      const auto compressed = zstd_compress(sequence).value();
      return encode_bytes(compressed);
    }
    default:
      throw TactileError {"Invalid compression strategy"};
  }
}

auto base64_decode_tiles(StringView tiles,
                         const TileExtent extent,
                         const TileCompression compression) -> TileMatrix
{
  const auto decoded = Base64::decode(tiles.data(), tiles.size());

  switch (compression) {
    case TileCompression::None:
      return restore_tiles(decoded, extent);

    case TileCompression::Zlib: {
      const auto decompressed = zlib_decompress(decoded.data(), decoded.size()).value();
      return restore_tiles(decompressed, extent);
    }
    case TileCompression::Zstd: {
      const auto decompressed = zstd_decompress(decoded.data(), decoded.size()).value();
      return restore_tiles(decompressed, extent);
    }
    default:
      throw TactileError {"Invalid compression strategy"};
  }
}

}  // namespace tactile::io