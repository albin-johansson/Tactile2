// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/io/compress/compression_format.hpp"
#include "tactile/zstd/api.hpp"

namespace tactile::zstd {

/**
 * Provides compression and decompression using the Zstandard algorithm.
 *
 * \see https://github.com/facebook/zstd
 */
class TACTILE_ZSTD_API ZstdCompressionFormat final : public ICompressionFormat
{
 public:
  [[nodiscard]]
  auto compress(ByteSpan input_data) const -> std::expected<ByteStream, ErrorCode> override;

  [[nodiscard]]
  auto decompress(ByteSpan input_data) const -> std::expected<ByteStream, ErrorCode> override;
};

}  // namespace tactile::zstd
