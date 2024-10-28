// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include <numeric>  // iota
#include <string>   // string

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tactile/zstd/zstd_compression_format.hpp"

namespace tactile::zstd {
namespace {

// tactile::zstd::ZstdCompressionFormat::compress
// tactile::zstd::ZstdCompressionFormat::decompress
TEST(ZstdCompressionFormat, CompressAndDecompressBytes)
{
  const ZstdCompressionFormat compressor {};

  ByteStream bytes {};
  bytes.resize(64'000);
  std::iota(bytes.begin(), bytes.end(), 0);

  const auto compressed_bytes = compressor.compress(bytes);
  ASSERT_TRUE(compressed_bytes.has_value());

  const auto decompressed_bytes = compressor.decompress(*compressed_bytes);
  ASSERT_TRUE(decompressed_bytes.has_value());
  EXPECT_THAT(*decompressed_bytes, testing::ContainerEq(bytes));
}

// tactile::zstd::ZstdCompressionFormat::compress
// tactile::zstd::ZstdCompressionFormat::decompress
TEST(ZstdCompressionFormat, CompressAndDecompressString)
{
  const ZstdCompressionFormat compressor {};

  constexpr std::string_view original_string =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Mi bibendum neque egestas congue quisque egestas diam in arcu. Varius duis at consectetur lorem. Ultricies tristique nulla aliquet enim tortor at auctor. Nibh nisl condimentum id venenatis a condimentum vitae sapien pellentesque. Venenatis urna cursus eget nunc scelerisque. Mattis molestie a iaculis at erat pellentesque adipiscing commodo elit. Commodo ullamcorper a lacus vestibulum sed arcu non odio euismod. Vivamus arcu felis bibendum ut. Libero enim sed faucibus turpis in eu mi bibendum neque. Blandit volutpat maecenas volutpat blandit aliquam etiam.";

  const auto compressed_bytes = compressor.compress(make_byte_span(original_string));
  ASSERT_TRUE(compressed_bytes.has_value());

  const auto decompressed_bytes = compressor.decompress(*compressed_bytes);
  ASSERT_TRUE(decompressed_bytes.has_value());

  const std::string restored_string {decompressed_bytes->begin(), decompressed_bytes->end()};
  EXPECT_EQ(restored_string, original_string);
}

}  // namespace
}  // namespace tactile::zstd
