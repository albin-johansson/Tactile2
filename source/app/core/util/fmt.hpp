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

#pragma once

#include <algorithm>    // min
#include <array>        // array
#include <sstream>      // stringstream
#include <string_view>  // string_view

#include <boost/stacktrace/stacktrace.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "core/common/fs.hpp"
#include "core/common/ints.hpp"

namespace fmt {

template <>
struct formatter<tactile::fs::path> : formatter<tactile::fs_string_view>
{
  auto format(const tactile::fs::path& path, auto& ctx) const
  {
    return formatter<tactile::fs_string_view>::format(path.c_str(), ctx);
  }
};

template <>
struct formatter<boost::stacktrace::stacktrace> : formatter<std::string_view>
{
  auto format(const boost::stacktrace::stacktrace& trace, auto& ctx) const
  {
    std::stringstream stream;
    stream << trace;
    return formatter<std::string_view>::format(stream.str(), ctx);
  }
};

static_assert(is_formattable<tactile::fs::path, tactile::fs::path::value_type>::value);
static_assert(is_formattable<boost::stacktrace::stacktrace, char>::value);

}  // namespace fmt

namespace tactile {

template <usize Capacity = 128>
class FmtString final
{
 public:
  template <typename... Args>
  explicit FmtString(const std::string_view fmt, const Args&... args)
  {
    const auto result =
        fmt::format_to_n(mBuffer.begin(), Capacity, fmt::runtime(fmt), args...);
    *result.out = '\0'; /* Ensure null-terminator */
    mSize = (std::min)(result.size, Capacity);
  }

  [[nodiscard]] auto data() const noexcept -> const char*
  {
    return mBuffer.data();
  }

  [[nodiscard]] auto view() const noexcept -> std::string_view
  {
    return std::string_view {mBuffer.data(), mSize};
  }

  [[nodiscard]] auto size() const noexcept -> usize
  {
    return view().size();
  }

  [[nodiscard]] constexpr auto capacity() const noexcept -> usize
  {
    return Capacity;
  }

 private:
  std::array<char, Capacity + 1> mBuffer;  // NOLINT
  usize                          mSize {};
};

}  // namespace tactile