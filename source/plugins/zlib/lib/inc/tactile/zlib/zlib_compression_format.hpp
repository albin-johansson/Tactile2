// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/io/compress/compression_format.hpp"
#include "tactile/zlib/api.hpp"

namespace tactile::zlib {

/**
 * Provides compression using the Zlib library.
 *
 * \see https://github.com/madler/zlib
 */
class TACTILE_ZLIB_API ZlibCompressionFormat final : public ICompressionFormat
{
 public:
  [[nodiscard]]
  auto compress(ByteSpan input_data) const -> std::expected<ByteStream, ErrorCode> override;

  [[nodiscard]]
  auto decompress(ByteSpan input_data) const -> std::expected<ByteStream, ErrorCode> override;
};

}  // namespace tactile::zlib
