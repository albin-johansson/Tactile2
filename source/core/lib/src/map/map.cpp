// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/map/map.hpp"

#include "tactile/base/io/save/ir.hpp"
#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/core/debug/assert.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer.hpp"
#include "tactile/core/layer/layer_common.hpp"
#include "tactile/core/layer/object_layer.hpp"
#include "tactile/core/layer/tile_layer.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map_spec.hpp"
#include "tactile/core/meta/meta.hpp"
#include "tactile/core/tile/tileset.hpp"
#include "tactile/core/tile/tileset_types.hpp"
#include "tactile/core/ui/viewport.hpp"

namespace tactile::core {

auto is_map(const Registry& registry, const EntityID entity) -> bool
{
  return registry.has<CMeta>(entity) &&           //
         registry.has<CMap>(entity) &&            //
         registry.has<CTileFormat>(entity) &&     //
         registry.has<CMapIdCache>(entity) &&     //
         registry.has<CLayerSuffixes>(entity) &&  //
         registry.has<CViewport>(entity);
}

auto make_map(Registry& registry, const MapSpec& spec) -> EntityID
{
  if (!is_valid(spec)) {
    TACTILE_CORE_ERROR("Tried to create map from invalid map specification");
    return kInvalidEntity;
  }

  const auto map_entity = registry.make_entity();

  registry.add<CMeta>(map_entity);

  auto& map = registry.add<CMap>(map_entity);
  map.orientation = spec.orientation;
  map.extent = spec.extent;
  map.tile_size = spec.tile_size;
  map.root_layer = make_group_layer(registry);
  map.active_layer = kInvalidEntity;
  map.active_tileset = kInvalidEntity;

  auto& format = registry.add<CTileFormat>(map_entity);
  format.encoding = TileEncoding::kPlainText;
  format.compression = std::nullopt;
  format.comp_level = std::nullopt;

  auto& id_cache = registry.add<CMapIdCache>(map_entity);
  id_cache.next_tile_id = TileID {1};
  id_cache.next_object_id = 1;
  id_cache.next_layer_id = 1;

  auto& suffixes = registry.add<CLayerSuffixes>(map_entity);
  suffixes.tile_layer_suffix = 1;
  suffixes.object_layer_suffix = 1;
  suffixes.group_layer_suffix = 1;

  auto& viewport = registry.add<CViewport>(map_entity);
  viewport.pos = Float2 {0, 0};
  viewport.size = Float2 {0, 0};
  viewport.scale = 1.0f;

  TACTILE_ASSERT(is_map(registry, map_entity));
  return map_entity;
}

auto make_map(Registry& registry, IRenderer& renderer, const ir::Map& ir_map)
    -> std::expected<EntityID, ErrorCode>
{
  const auto map_id = registry.make_entity();

  registry.add<CMeta>(map_id);
  convert_ir_metadata(registry, map_id, ir_map.meta);

  auto& map = registry.add<CMap>(map_id);
  map.orientation = TileOrientation::kOrthogonal;  // TODO
  map.extent = ir_map.extent;
  map.tile_size = ir_map.tile_size;
  map.root_layer = make_group_layer(registry);
  map.active_layer = kInvalidEntity;
  map.active_tileset = kInvalidEntity;

  auto& format = registry.add<CTileFormat>(map_id);
  format.encoding = ir_map.tile_format.encoding;
  format.compression = ir_map.tile_format.compression;
  format.comp_level = ir_map.tile_format.compression_level;

  auto& id_cache = registry.add<CMapIdCache>(map_id);
  id_cache.next_tile_id = TileID {1};  // TODO
  id_cache.next_object_id = ir_map.next_object_id;
  id_cache.next_layer_id = ir_map.next_layer_id;

  auto& suffixes = registry.add<CLayerSuffixes>(map_id);
  suffixes.tile_layer_suffix = 1;
  suffixes.object_layer_suffix = 1;
  suffixes.group_layer_suffix = 1;

  auto& viewport = registry.add<CViewport>(map_id);
  viewport.pos = Float2 {0, 0};
  viewport.size = Float2 {0, 0};
  viewport.scale = 1.0f;

  // TODO components

  map.attached_tilesets.reserve(ir_map.tilesets.size());
  for (const auto& ir_tileset_ref : ir_map.tilesets) {
    const auto tileset_id = make_tileset(registry, renderer, ir_tileset_ref);

    if (!tileset_id.has_value()) {
      return std::unexpected {tileset_id.error()};
    }

    map.attached_tilesets.push_back(*tileset_id);
  }

  auto& root_layer = registry.get<CGroupLayer>(map.root_layer);
  root_layer.layers.reserve(ir_map.layers.size());

  for (const auto& ir_layer : ir_map.layers) {
    root_layer.layers.push_back(make_layer(registry, ir_layer));
  }

  TACTILE_ASSERT(is_map(registry, map_id));
  return map_id;
}

void destroy_map(Registry& registry, const EntityID map_entity)
{
  TACTILE_ASSERT(is_map(registry, map_entity));

  auto& map = registry.get<CMap>(map_entity);
  auto& root_layer = registry.get<CGroupLayer>(map.root_layer);

  for (const auto layer_entity : root_layer.layers) {
    if (registry.has<CGroupLayer>(layer_entity)) {
      destroy_group_layer(registry, layer_entity);
    }
    else if (registry.has<CObjectLayer>(layer_entity)) {
      destroy_object_layer(registry, layer_entity);
    }
    else {
      destroy_tile_layer(registry, layer_entity);
    }
  }

  registry.destroy(map.root_layer);
  registry.destroy(map_entity);
}

auto add_tileset_to_map(Registry& registry,
                        const EntityID map_id,
                        const TilesetSpec& tileset_spec) -> std::expected<EntityID, ErrorCode>
{
  TACTILE_ASSERT(is_map(registry, map_id));

  auto& map = registry.get<CMap>(map_id);
  auto& id_cache = registry.get<CMapIdCache>(map_id);

  const auto tileset_id = make_tileset_instance(registry, tileset_spec, id_cache.next_tile_id);
  if (!tileset_id.has_value()) {
    return std::unexpected {tileset_id.error()};
  }

  const auto& tileset = registry.get<CTileset>(*tileset_id);
  id_cache.next_tile_id += saturate_cast<TileID>(tileset.tiles.size());

  map.attached_tilesets.push_back(*tileset_id);
  map.active_tileset = *tileset_id;

  TACTILE_CORE_DEBUG("Added tileset {} to map {}",
                    entity_to_string(*tileset_id),
                    entity_to_string(map_id));
  return tileset_id;
}

void remove_tileset_from_map(Registry& registry,
                             const EntityID map_id,
                             const EntityID tileset_id)
{
  TACTILE_ASSERT(is_map(registry, map_id));
  TACTILE_ASSERT(is_tileset_instance(registry, tileset_id));

  TACTILE_CORE_DEBUG("Removing tileset {} from map {}",
                    entity_to_string(tileset_id),
                    entity_to_string(map_id));

  // Note, the tile range used by the tileset is intentionally not returned to keep the logic
  // as simple as possible. It also makes implementing tileset commands easier.

  auto& map = registry.get<CMap>(map_id);
  std::erase(map.attached_tilesets, tileset_id);

  if (map.active_tileset == tileset_id) {
    map.active_tileset = !map.attached_tilesets.empty() ? map.attached_tilesets.front()  //
                                                        : kInvalidEntity;
  }
}

auto add_layer_to_map(Registry& registry, const EntityID map_id, const LayerType type)
    -> std::expected<EntityID, ErrorCode>
{
  TACTILE_ASSERT(is_map(registry, map_id));
  const auto& map = registry.get<CMap>(map_id);

  EntityID layer_entity {};
  switch (type) {
    case LayerType::kTileLayer:   layer_entity = make_tile_layer(registry, map.extent); break;
    case LayerType::kObjectLayer: layer_entity = make_object_layer(registry); break;
    case LayerType::kGroupLayer:  layer_entity = make_group_layer(registry); break;
    default:                      return std::unexpected {ErrorCode::kBadParam};
  }

  append_layer_to_map(registry, map_id, layer_entity);

  return layer_entity;
}

void append_layer_to_map(Registry& registry, const EntityID map_id, const EntityID layer_id)
{
  TACTILE_ASSERT(is_map(registry, map_id));
  TACTILE_ASSERT(is_layer(registry, layer_id));

  auto& map = registry.get<CMap>(map_id);

  if (is_group_layer(registry, map.active_layer)) {
    auto& active_group = registry.get<CGroupLayer>(map.active_layer);
    active_group.layers.push_back(layer_id);
  }
  else {
    auto& root = registry.get<CGroupLayer>(map.root_layer);
    root.layers.push_back(layer_id);
  }

  if (layer_id != kInvalidEntity) {
    map.active_layer = layer_id;
  }
}

auto remove_layer_from_map(Registry& registry, const EntityID map_id, const EntityID layer_id)
    -> std::expected<void, ErrorCode>
{
  TACTILE_ASSERT(is_map(registry, map_id));
  TACTILE_ASSERT(is_layer(registry, layer_id));

  auto& map = registry.get<CMap>(map_id);

  const auto parent_layer_id = find_parent_layer(registry, map.root_layer, layer_id);
  if (parent_layer_id == kInvalidEntity) {
    return std::unexpected {ErrorCode::kBadParam};
  }

  auto& parent_layer = registry.get<CGroupLayer>(*parent_layer_id);
  std::erase(parent_layer.layers, layer_id);

  if (map.active_layer == layer_id) {
    map.active_layer = kInvalidEntity;
  }

  return {};
}

}  // namespace tactile::core
