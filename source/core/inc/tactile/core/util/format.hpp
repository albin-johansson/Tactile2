// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <format>       // format_to_n, vformat_to, make_format_args
#include <iterator>     // back_inserter
#include <type_traits>  // type_identity_t
#include <utility>      // forward
#include <version>      // __cpp_lib_format

#include "tactile/base/container/string.hpp"
#include "tactile/base/int.hpp"
#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/core/util/memory_buffer.hpp"

// Despite Clang having implemented most of std::format, they don't (yet)
// provide the associated version macros. Making it difficult to write portable
// version checks, so we have to do some guess work here.
#if TACTILE_COMPILER_CLANG && !defined(__cpp_lib_format)
  #define TACTILE_HAS_STD_FORMAT_STRING 0
#else
  #if __cpp_lib_format >= 202207L
    #define TACTILE_HAS_STD_FORMAT_STRING 1
  #else
    #define TACTILE_HAS_STD_FORMAT_STRING 0
  #endif
#endif

namespace tactile {

#if TACTILE_HAS_STD_FORMAT_STRING

template <typename... Args>
using FormatString = std::format_string<std::type_identity_t<Args>...>;

#else

template <typename... Args>
using FormatString = StringView;

#endif

/**
 * Formats a string into a given memory buffer.
 *
 * \details
 * The formatted string will be truncated if it doesn't fit in the buffer.
 *
 * \tparam N    The maximum number of elements in the buffer.
 * \tparam Args The format argument types.
 *
 * \param buffer The target memory buffer.
 * \param fmt    The format string.
 * \param args   The format arguments.
 */
template <usize N, typename... Args>
void format_to_buffer(MemoryBuffer<char, N>& buffer,
                      const FormatString<Args...> fmt,
                      const Args&... args)
{
  const auto remaining_chars = buffer.remaining_capacity();
  if (remaining_chars < 1) {
    return;
  }

#if TACTILE_HAS_STD_FORMAT_STRING
  std::format_to_n(std::back_inserter(buffer),
                   saturate_cast<ssize>(remaining_chars),
                   fmt,
                   std::forward<Args>(args)...);
#else
  // We have to use vformat_to to avoid compile-time checks, because Clang might
  // actually feature such checks even though their library doesn't announce it.
  // This is safe because the MemoryBuffer::push_back function has no effect
  // when the buffer is full.
  std::vformat_to(std::back_inserter(buffer),
                  fmt,
                  std::make_format_args(args...));
#endif
}

}  // namespace tactile
