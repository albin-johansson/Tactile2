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

#include "viewport_system.hpp"

#include <algorithm>  // min, max
#include <cmath>      // round
#include <utility>    // pair

#include "core/components/texture.hpp"
#include "core/map.hpp"
#include "core/viewport.hpp"
#include "misc/assert.hpp"
#include "registry_system.hpp"

namespace tactile::sys {
namespace {

constexpr float gMinTileHeight = 4;

[[nodiscard]] auto GetViewportOffsetDelta(const float tileWidth, const float ratio)
    -> std::pair<float, float>
{
  const auto dx = std::round((std::max)(2.0f, tileWidth * 0.05f));
  const auto dy = dx / ratio;

  return {dx, dy};
}

}  // namespace

void OffsetViewport(entt::registry& registry, const float dx, const float dy)
{
  auto& viewport = registry.ctx<Viewport>();
  viewport.x_offset += dx;
  viewport.y_offset += dy;
}

void OffsetBoundViewport(entt::registry& registry,
                         const entt::entity entity,
                         const float dx,
                         const float dy,
                         const float viewWidth,
                         const float viewHeight)
{
  auto& viewport = checked_get<Viewport>(registry, entity);
  viewport.x_offset += dx;
  viewport.y_offset += dy;

  viewport.x_offset = (std::min)(0.0f, viewport.x_offset);
  viewport.y_offset = (std::min)(0.0f, viewport.y_offset);

  const auto& texture = checked_get<comp::Texture>(registry, entity);
  const auto textureWidth = static_cast<float>(texture.width);
  const auto textureHeight = static_cast<float>(texture.height);

  viewport.x_offset = (std::max)(-textureWidth + viewWidth, viewport.x_offset);
  viewport.y_offset = (std::max)(-textureHeight + viewHeight, viewport.y_offset);
}

void PanViewportLeft(entt::registry& registry)
{
  auto& viewport = registry.ctx<Viewport>();
  viewport.x_offset += viewport.tile_width;
}

void PanViewportRight(entt::registry& registry)
{
  auto& viewport = registry.ctx<Viewport>();
  viewport.x_offset -= viewport.tile_width;
}

void PanViewportUp(entt::registry& registry)
{
  auto& viewport = registry.ctx<Viewport>();
  viewport.y_offset += viewport.tile_height;
}

void PanViewportDown(entt::registry& registry)
{
  auto& viewport = registry.ctx<Viewport>();
  viewport.y_offset -= viewport.tile_height;
}

void ResetViewportZoom(entt::registry& registry)
{
  const auto& map = registry.ctx<MapInfo>();
  auto& viewport = registry.ctx<Viewport>();
  viewport.tile_width = 2.0f * static_cast<float>(map.tile_width);
  viewport.tile_height = 2.0f * static_cast<float>(map.tile_height);
}

void DecreaseViewportZoom(entt::registry& registry,
                          const float mouseX,
                          const float mouseY)
{
  TACTILE_ASSERT(CanDecreaseViewportZoom(registry));

  auto& viewport = registry.ctx<Viewport>();

  // Percentages of map to the left of and above the cursor
  const auto px = (mouseX - viewport.x_offset) / viewport.tile_width;
  const auto py = (mouseY - viewport.y_offset) / viewport.tile_height;

  {
    const auto ratio = viewport.tile_width / viewport.tile_height;
    const auto [dx, dy] = GetViewportOffsetDelta(viewport.tile_width, ratio);
    viewport.tile_width -= dx;
    viewport.tile_height -= dy;

    viewport.tile_width = (std::max)(gMinTileHeight * ratio, viewport.tile_width);
    viewport.tile_height = (std::max)(gMinTileHeight, viewport.tile_height);
  }

  viewport.x_offset = mouseX - (px * viewport.tile_width);
  viewport.y_offset = mouseY - (py * viewport.tile_height);
}

void IncreaseViewportZoom(entt::registry& registry,
                          const float mouseX,
                          const float mouseY)
{
  auto& viewport = registry.ctx<Viewport>();

  // Percentages of map to the left of and above the cursor
  const auto px = (mouseX - viewport.x_offset) / viewport.tile_width;
  const auto py = (mouseY - viewport.y_offset) / viewport.tile_height;

  {
    const auto ratio = viewport.tile_width / viewport.tile_height;
    const auto [dx, dy] = GetViewportOffsetDelta(viewport.tile_width, ratio);
    viewport.tile_width += dx;
    viewport.tile_height += dy;
  }

  viewport.x_offset = mouseX - (px * viewport.tile_width);
  viewport.y_offset = mouseY - (py * viewport.tile_height);
}

auto CanDecreaseViewportZoom(const entt::registry& registry) -> bool
{
  const auto& viewport = registry.ctx<Viewport>();
  return viewport.tile_height > gMinTileHeight;
}

auto GetViewportScalingRatio(const entt::registry& registry) -> ViewportScalingRatio
{
  const auto& viewport = registry.ctx<Viewport>();
  const auto& map = registry.ctx<MapInfo>();

  const auto xRatio = viewport.tile_width / static_cast<float>(map.tile_width);
  const auto yRatio = viewport.tile_height / static_cast<float>(map.tile_height);

  return {xRatio, yRatio};
}

}  // namespace tactile::sys
