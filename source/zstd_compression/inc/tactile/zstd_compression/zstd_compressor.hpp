// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/io/compress/compression_provider.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/zstd_compression/api.hpp"

namespace tactile {

/**
 * Provides compression and decompression using the Zstandard algorithm.
 *
 * \see https://github.com/facebook/zstd
 */
class TACTILE_ZSTD_API ZstdCompressor final : public ICompressionProvider
{
 public:
  [[nodiscard]]
  auto compress(ByteSpan input_data) const -> Result<ByteStream> override;

  [[nodiscard]]
  auto decompress(ByteSpan input_data) const -> Result<ByteStream> override;
};

}  // namespace tactile