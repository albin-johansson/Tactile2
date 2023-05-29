/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2023 Albin Johansson
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

#include <string_view>  // string_view

#include <fmt/core.h>
#include <magic_enum.hpp>

namespace tactile {

template <typename E>
struct MagicEnumFormatter : fmt::formatter<std::string_view> {
  auto format(const E value, auto& ctx) const
  {
    return fmt::format_to(ctx.out(), "{}", magic_enum::enum_name(value));
  }
};

}  // namespace tactile

#define TACTILE_MAGIC_ENUM_FORMATTER(Enum)                                  \
  template <>                                                               \
  struct fmt::formatter<Enum> final : tactile::MagicEnumFormatter<Enum> {}; \
  static_assert(fmt::is_formattable<Enum>::value)
