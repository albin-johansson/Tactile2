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

#include <imgui.h>

#include "common/debug/assert.hpp"
#include "common/numeric.hpp"
#include "common/type/math.hpp"
#include "core/color.hpp"
#include "core/tile/tile_pos.hpp"

namespace tactile {

[[nodiscard]] constexpr auto to_u32(const Color& color) noexcept -> uint32
{
  return IM_COL32(color.red, color.green, color.blue, color.alpha);
}

[[nodiscard]] inline auto to_color(const ImVec4& vec) -> Color
{
  return Color::from_norm(vec.x, vec.y, vec.z, vec.w);
}

[[nodiscard]] constexpr auto as_imvec2(const Float2 vec) noexcept -> ImVec2
{
  return {vec.x, vec.y};
}

[[nodiscard]] constexpr auto as_imvec2(const Int2 vec) noexcept -> ImVec2
{
  return {static_cast<float>(vec.x), static_cast<float>(vec.y)};
}

[[nodiscard]] constexpr auto as_float2(const ImVec2 vec) noexcept -> Float2
{
  return {vec.x, vec.y};
}

/// Converts a normalized opacity value to a value in the interval [0, 255].
[[nodiscard]] constexpr auto opacity_cast(const float opacity) noexcept -> uint8
{
  TACTILE_ASSERT(opacity >= 0.0f);
  TACTILE_ASSERT(opacity <= 1.0f);
  return static_cast<uint8>(255.0f * opacity);
}

}  // namespace tactile
