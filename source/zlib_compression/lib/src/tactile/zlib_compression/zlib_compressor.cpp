// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/zlib_compression/zlib_compressor.hpp"

#include <array>         // array
#include <system_error>  // errc, make_error_code
#include <utility>       // move

#define Z_PREFIX_SET
#include <zlib.h>

#include "tactile/base/container/expected.hpp"
#include "tactile/base/io/byte_stream.hpp"
#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/runtime/logging.hpp"

namespace tactile {
namespace zlib_compressor {

// Zlib has inconsistently named typedefs, we use these instead.
using z_byte = ::Bytef;
using z_uint = ::uInt;
using z_ulong = ::uLong;

/**
 * Type used for local "staging" buffers, used to batch data processing.
 */
using StagingBuffer = std::array<z_byte, 16'384>;

/**
 * Provides callbacks that controls the behavior of stream processing functions.
 */
struct ZlibCallbacks final
{
  using init_stream_func = int (*)(z_stream*);
  using process_func = int (*)(z_stream*, int);
  using end_func = int (*)(z_stream*);

  init_stream_func init_stream;
  process_func process_stream;
  end_func end_stream;
};

/**
 * Initializes a Zlib stream.
 *
 * \param      callbacks      The associated callbacks.
 * \param      input_data     The input data that will be processed.
 * \param      staging_buffer The staging buffer that will be used.
 * \param[out] stream         The resulting stream.
 *
 * \return
 * Nothing if successful; an error code otherwise.
 */
[[nodiscard]]
auto init_stream(const ZlibCallbacks& callbacks,
                 const ByteSpan input_data,
                 StagingBuffer& staging_buffer,
                 z_stream& stream) -> Result<void>
{
  stream.next_in = const_cast<z_byte*>(input_data.data());  // NOLINT
  stream.avail_in = saturate_cast<z_uint>(input_data.size_bytes());
  stream.next_out = staging_buffer.data();
  stream.avail_out = saturate_cast<z_uint>(staging_buffer.size());

  const auto init_stream_result = callbacks.init_stream(&stream);
  if (init_stream_result != Z_OK) {
    log(LogLevel::kError, "Could not initialize z_stream: {}", zError(init_stream_result));
    return unexpected(std::make_error_code(std::errc::io_error));
  }

  return kOK;
}

/**
 * Processes a stream in chunks.
 *
 * \param      callbacks      The associated callbacks.
 * \param      stream         The associated stream.
 * \param      staging_buffer The staging buffer that will be used.
 * \param[out] output_buffer  The target output buffer.
 *
 * \return
 * Nothing if successful; an error code otherwise.
 */
[[nodiscard]]
auto process_stream(const ZlibCallbacks& callbacks,
                    z_stream& stream,
                    StagingBuffer& staging_buffer,
                    ByteStream& output_buffer) -> Result<void>
{
  const auto copy_processed_batch_to_output_buffer = [&] {
    const auto written_bytes = staging_buffer.size() - stream.avail_out;
    output_buffer.insert(output_buffer.end(),
                         staging_buffer.data(),
                         staging_buffer.data() + written_bytes);
  };

  while (true) {
    const auto process_result = callbacks.process_stream(&stream, Z_FINISH);

    if (process_result == Z_STREAM_END) {
      copy_processed_batch_to_output_buffer();
      break;
    }

    if (process_result == Z_OK || process_result == Z_BUF_ERROR) {
      log(LogLevel::kTrace, "Flushing and resetting output buffer");

      // We ran out of space in the staging buffer, so we need to flush and
      // reuse it.
      copy_processed_batch_to_output_buffer();
      stream.next_out = staging_buffer.data();
      stream.avail_out = saturate_cast<z_uint>(staging_buffer.size());
    }
    else {
      log(LogLevel::kError, "Could not process Zlib chunk: {}", zError(process_result));
      return unexpected(std::make_error_code(std::errc::io_error));
    }
  }

  return kOK;
}

/**
 * Finalizes a Zlib stream.
 *
 * \param callbacks The associated callbacks.
 * \param stream    The associated stream.
 *
 * \return
 * Nothing if successful; an error code otherwise.
 */
[[nodiscard]]
auto end_stream(const ZlibCallbacks& callbacks, z_stream& stream) -> Result<void>
{
  const auto end_stream_result = callbacks.end_stream(&stream);

  if (end_stream_result != Z_OK) {
    log(LogLevel::kError, "Could not finalize z_stream: {}", zError(end_stream_result));
    return unexpected(std::make_error_code(std::errc::io_error));
  }

  return kOK;
}

}  // namespace zlib_compressor

auto ZlibCompressor::compress(const ByteSpan input_data) const -> Result<ByteStream>
{
  zlib_compressor::ZlibCallbacks callbacks {};
  callbacks.init_stream = [](z_stream* stream) {
    return z_deflateInit(stream, Z_DEFAULT_COMPRESSION);
  };
  callbacks.process_stream = &deflate;
  callbacks.end_stream = &deflateEnd;

  z_stream stream {};

  zlib_compressor::StagingBuffer staging_buffer;  // NOLINT uninitialized
  ByteStream output_buffer {};

  return zlib_compressor::init_stream(callbacks, input_data, staging_buffer, stream)
      .and_then([&] {
        const usize output_buffer_bound =
            deflateBound(&stream,
                         saturate_cast<zlib_compressor::z_ulong>(input_data.size_bytes()));
        output_buffer.reserve(output_buffer_bound);
        return zlib_compressor::process_stream(callbacks,
                                               stream,
                                               staging_buffer,
                                               output_buffer);
      })
      .and_then([&] { return zlib_compressor::end_stream(callbacks, stream); })
      .transform([&] { return std::move(output_buffer); });
}

auto ZlibCompressor::decompress(const ByteSpan input_data) const -> Result<ByteStream>
{
  zlib_compressor::ZlibCallbacks callbacks {};
  callbacks.init_stream = [](z_stream* stream) { return z_inflateInit(stream); };
  callbacks.process_stream = &inflate;
  callbacks.end_stream = &inflateEnd;

  z_stream stream {};

  zlib_compressor::StagingBuffer staging_buffer;  // NOLINT uninitialized
  ByteStream output_buffer {};

  return zlib_compressor::init_stream(callbacks, input_data, staging_buffer, stream)
      .and_then([&] {
        output_buffer.reserve(2'048);
        return zlib_compressor::process_stream(callbacks,
                                               stream,
                                               staging_buffer,
                                               output_buffer);
      })
      .and_then([&] { return zlib_compressor::end_stream(callbacks, stream); })
      .transform([&] { return std::move(output_buffer); });
}

}  // namespace tactile
