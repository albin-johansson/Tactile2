#include "viewport_system.hpp"

#include <algorithm>  // max
#include <cassert>    // assert
#include <cmath>      // round
#include <utility>    // pair

#include "core/map.hpp"
#include "core/viewport.hpp"

namespace Tactile::Sys {
namespace {

constexpr float min_tile_height = 4;

[[nodiscard]] auto GetViewportOffsetDelta(const float tileWidth, const float ratio)
    -> std::pair<float, float>
{
  const auto dx = std::round(std::max(2.0f, tileWidth * 0.05f));
  const auto dy = dx / ratio;

  return {dx, dy};
}

}  // namespace

void OffsetViewport(entt::registry& registry, float dx, float dy)
{
  auto& viewport = registry.ctx<Viewport>();
  viewport.x_offset += dx;
  viewport.y_offset += dy;
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
  const auto& map = registry.ctx<Map>();
  auto& viewport = registry.ctx<Viewport>();
  viewport.tile_width = 2.0f * static_cast<float>(map.tile_width);
  viewport.tile_height = 2.0f * static_cast<float>(map.tile_height);
}

void DecreaseViewportZoom(entt::registry& registry)
{
  auto& viewport = registry.ctx<Viewport>();
  assert(CanDecreaseViewportZoom(registry));

  const auto ratio = viewport.tile_width / viewport.tile_height;
  const auto [dx, dy] = GetViewportOffsetDelta(viewport.tile_width, ratio);
  viewport.tile_width -= dx;
  viewport.tile_height -= dy;

  viewport.tile_width = std::max(min_tile_height * ratio, viewport.tile_width);
  viewport.tile_height = std::max(min_tile_height, viewport.tile_height);
}

void IncreaseViewportZoom(entt::registry& registry)
{
  auto& viewport = registry.ctx<Viewport>();

  const auto ratio = viewport.tile_width / viewport.tile_height;
  const auto [dx, dy] = GetViewportOffsetDelta(viewport.tile_width, ratio);
  viewport.tile_width += dx;
  viewport.tile_height += dy;
}

auto CanDecreaseViewportZoom(const entt::registry& registry) -> bool
{
  const auto& viewport = registry.ctx<Viewport>();
  return viewport.tile_height > min_tile_height;
}

}  // namespace Tactile::Sys
