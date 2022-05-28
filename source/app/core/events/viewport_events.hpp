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

#include <entt/entity/entity.hpp>

namespace tactile {

/// \addtogroup events
/// \{

struct CenterViewportEvent final
{};

struct ResetZoomEvent final
{};

struct DecreaseZoomEvent final
{};

struct IncreaseZoomEvent final
{};

struct OffsetViewportEvent final
{
  float dx{};
  float dy{};
};

/**
 * \brief Offset a viewport that cannot exceed the bounds of the shown content.
 */
struct OffsetBoundViewportEvent final
{
  entt::entity entity{entt::null};
  float dx{};
  float dy{};
  float view_width{};
  float view_height{};
};

struct PanUpEvent final
{};

struct PanDownEvent final
{};

struct PanLeftEvent final
{};

struct PanRightEvent final
{};

/// \} End of group events

}  // namespace tactile