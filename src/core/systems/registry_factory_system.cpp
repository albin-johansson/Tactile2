#include "registry_factory_system.hpp"

#include "core/components/layer.hpp"
#include "core/components/property_context.hpp"
#include "core/components/tileset.hpp"
#include "core/map.hpp"
#include "core/viewport.hpp"

namespace Tactile::Sys {

auto MakeRegistry() -> entt::registry
{
  entt::registry registry;

  registry.set<Map>();
  registry.set<ActiveLayer>();
  registry.set<ActiveTileset>();
  registry.set<ActivePropertyContext>();

  auto& viewport = registry.set<Viewport>();
  viewport.tile_width = 64;
  viewport.tile_height = 64;
  viewport.x_offset = 0;
  viewport.y_offset = 0;

  auto& context = registry.set<PropertyContext>();
  context.name = "Map";

  return registry;
}

}  // namespace Tactile::Sys
