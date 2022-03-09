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

#include "tileset_system.hpp"

#include <unordered_map>  // unordered_map
#include <utility>        // move

#include "context_system.hpp"
#include "core/components/animation.hpp"
#include "core/components/attributes.hpp"
#include "core/components/texture.hpp"
#include "core/components/tiles.hpp"
#include "core/utils/tiles.hpp"
#include "core/viewport.hpp"
#include "misc/assert.hpp"
#include "misc/logging.hpp"
#include "misc/throw.hpp"
#include "registry_system.hpp"

namespace tactile::sys {
namespace {

[[nodiscard]] auto create_source_rect_cache(const comp::Tileset& tileset)
    -> std::unordered_map<tile_id, cen::irect>
{
  std::unordered_map<tile_id, cen::irect> cache;

  const auto amount = (tileset.last_id + 1) - tileset.first_id;
  cache.reserve(static_cast<usize>(amount));

  for (tile_id id{tileset.first_id}; id <= tileset.last_id; ++id) {
    const auto index = id - tileset.first_id;

    const auto [row, col] = to_matrix_coords(index, tileset.column_count);
    const auto x = col * tileset.tile_width;
    const auto y = row * tileset.tile_height;

    cache.emplace(id, cen::irect{x, y, tileset.tile_width, tileset.tile_height});
  }

  return cache;
}

void refresh_tileset_cache(entt::registry& registry, const entt::entity entity)
{
  const auto& tileset = checked_get<comp::Tileset>(registry, entity);

  auto& cache = registry.emplace_or_replace<comp::TilesetCache>(entity);
  cache.source_rects = create_source_rect_cache(tileset);

  for (auto&& [tileEntity, tile] : registry.view<comp::MetaTile>().each()) {
    if (tile.id >= tileset.first_id && tile.id <= tileset.last_id) {
      cache.tiles.try_emplace(tile.id, tileEntity);
    }
  }
}

void register_new_tiles_in_tile_context(entt::registry& registry,
                                        const entt::entity tilesetEntity)
{
  auto& tilesets = registry.ctx<comp::TilesetContext>();
  const auto& tileset = checked_get<comp::Tileset>(registry, tilesetEntity);

  for (tile_id tile = tileset.first_id; tile <= tileset.last_id; ++tile) {
    tilesets.tile_to_tileset[tile] = tilesetEntity;
  }
}

void unregister_tiles_from_tile_context(entt::registry& registry,
                                        const entt::entity tilesetEntity)
{
  auto& tilesets = registry.ctx<comp::TilesetContext>();
  const auto& tileset = checked_get<comp::Tileset>(registry, tilesetEntity);

  for (tile_id tile = tileset.first_id; tile <= tileset.last_id; ++tile) {
    tilesets.tile_to_tileset.erase(tile);
  }
}

void add_viewport(entt::registry& registry,
                  const entt::entity tilesetEntity,
                  const int32 tileWidth,
                  const int32 tileHeight)
{
  auto& viewport = registry.emplace<Viewport>(tilesetEntity);
  viewport.x_offset = 0;
  viewport.y_offset = 0;
  viewport.tile_width = static_cast<float>(tileWidth);
  viewport.tile_height = static_cast<float>(tileHeight);
}

}  // namespace

void update_tilesets(entt::registry& registry)
{
  for (auto&& [entity, cache] : registry.view<comp::TilesetCache>().each()) {
    cache.source_to_render.clear();
  }
}

auto make_tileset(entt::registry& registry,
                  const tile_id firstId,
                  const comp::Texture& texture,
                  const int32 tileWidth,
                  const int32 tileHeight) -> entt::entity
{
  auto& tilesets = registry.ctx<comp::TilesetContext>();

  const auto id = tilesets.next_id;
  ++tilesets.next_id;

  log_debug("Creating tileset with ID '{}'", id);

  const auto tilesetEntity = registry.create();
  auto& tileset = registry.emplace<comp::Tileset>(tilesetEntity);
  tileset.id = id;

  tileset.tile_width = tileWidth;
  tileset.tile_height = tileHeight;

  tileset.row_count = texture.height / tileHeight;
  tileset.column_count = texture.width / tileWidth;
  tileset.tile_count = tileset.row_count * tileset.column_count;

  tileset.first_id = firstId;
  tileset.last_id = tileset.first_id + tile_id{tileset.tile_count};
  tilesets.next_tile_id += tileset.tile_count + 1;

  registry.emplace<comp::Texture>(tilesetEntity, texture);

  auto& uv = registry.emplace<comp::UvTileSize>(tilesetEntity);
  uv.width = static_cast<float>(tileWidth) / static_cast<float>(texture.width);
  uv.height = static_cast<float>(tileHeight) / static_cast<float>(texture.height);

  auto& cache = registry.emplace<comp::TilesetCache>(tilesetEntity);
  cache.source_rects = create_source_rect_cache(tileset);

  auto& context = add_attribute_context(registry, tilesetEntity);
  context.name = texture.path.stem().string();

  registry.emplace<comp::TilesetSelection>(tilesetEntity);

  add_viewport(registry, tilesetEntity, tileset.tile_width, tileset.tile_height);

  register_new_tiles_in_tile_context(registry, tilesetEntity);

  return tilesetEntity;
}

auto make_tileset(entt::registry& registry,
                  const comp::Texture& texture,
                  const int32 tileWidth,
                  const int32 tileHeight) -> entt::entity
{
  auto& context = registry.ctx<comp::TilesetContext>();
  return make_tileset(registry, context.next_tile_id, texture, tileWidth, tileHeight);
}

auto restore_tileset(entt::registry& registry, TilesetSnapshot snapshot) -> entt::entity
{
  log_debug("Restoring tileset with ID '{}'", snapshot.core.id);
  const auto tilesetEntity = registry.create();

  auto& tileset = registry.emplace<comp::Tileset>(tilesetEntity, snapshot.core);
  registry.emplace<comp::TilesetSelection>(tilesetEntity, snapshot.selection);
  registry.emplace<comp::Texture>(tilesetEntity, snapshot.texture);
  registry.emplace<comp::UvTileSize>(tilesetEntity, snapshot.uv);

  add_viewport(registry, tilesetEntity, tileset.tile_width, tileset.tile_height);

  refresh_tileset_cache(registry, tilesetEntity);
  register_new_tiles_in_tile_context(registry, tilesetEntity);

  restore_attribute_context(registry, tilesetEntity, std::move(snapshot.context));

  return tilesetEntity;
}

auto copy_tileset(const entt::registry& registry, const entt::entity source)
    -> TilesetSnapshot
{
  TACTILE_ASSERT(source != entt::null);
  TilesetSnapshot snapshot;

  snapshot.core = checked_get<comp::Tileset>(registry, source);
  snapshot.selection = checked_get<comp::TilesetSelection>(registry, source);
  snapshot.texture = checked_get<comp::Texture>(registry, source);
  snapshot.uv = checked_get<comp::UvTileSize>(registry, source);
  snapshot.context = copy_attribute_context(registry, source);

  return snapshot;
}

void select_tileset(entt::registry& registry, const tileset_id id)
{
  log_verbose("Selecting tileset '{}'", id);

  const auto entity = find_tileset(registry, id);
  TACTILE_ASSERT(entity != entt::null);

  auto& activeTileset = registry.ctx<comp::ActiveTileset>();
  activeTileset.entity = entity;
}

void remove_tileset(entt::registry& registry, const tileset_id id)
{
  log_debug("Removing tileset '{}'", id);

  const auto entity = find_tileset(registry, id);
  TACTILE_ASSERT(entity != entt::null);

  auto& activeTileset = registry.ctx<comp::ActiveTileset>();
  if (entity == activeTileset.entity) {
    activeTileset.entity = entt::null;
  }

  auto& activeContext = registry.ctx<comp::ActiveAttributeContext>();
  if (entity == activeContext.entity) {
    activeContext.entity = entt::null;
  }

  unregister_tiles_from_tile_context(registry, entity);

  registry.destroy(entity);

  if (!registry.view<comp::Tileset>().empty()) {
    activeTileset.entity = registry.view<comp::Tileset>().front();
  }
}

void update_tileset_selection(entt::registry& registry, const Region& region)
{
  auto& active = registry.ctx<comp::ActiveTileset>();
  TACTILE_ASSERT(active.entity != entt::null);

  auto& selection = checked_get<comp::TilesetSelection>(registry, active.entity);
  selection.region = region;
}

auto find_tileset(const entt::registry& registry, const tileset_id id) -> entt::entity
{
  for (auto&& [entity, tileset] : registry.view<comp::Tileset>().each()) {
    if (tileset.id == id) {
      return entity;
    }
  }

  return entt::null;
}

auto get_tileset_entity(const entt::registry& registry, const tileset_id id)
    -> entt::entity
{
  const auto entity = find_tileset(registry, id);
  if (entity != entt::null) {
    return entity;
  }
  else {
    throw_traced(tactile_error{"Invalid tileset ID!"});
  }
}

auto find_tile(const entt::registry& registry, const tile_id id) -> entt::entity
{
  for (auto&& [entity, fancy] : registry.view<comp::MetaTile>().each()) {
    if (fancy.id == id) {
      return entity;
    }
  }

  return entt::null;
}

auto get_tile_entity(const entt::registry& registry, const tile_id id) -> entt::entity
{
  const auto entity = find_tile(registry, id);
  if (entity != entt::null) {
    return entity;
  }
  else {
    throw_traced(tactile_error{"Invalid tile ID!"});
  }
}

auto find_tileset_with_tile(const entt::registry& registry, const tile_id id)
    -> entt::entity
{
  for (auto&& [entity, tileset] : registry.view<comp::Tileset>().each()) {
    if (id >= tileset.first_id && id <= tileset.last_id) {
      return entity;
    }
  }

  return entt::null;
}

auto find_active_tileset(const entt::registry& registry) -> entt::entity
{
  const auto& active = registry.ctx<comp::ActiveTileset>();
  return active.entity;
}

auto is_tileset_selection_not_empty(const entt::registry& registry) -> bool
{
  const auto& active = registry.ctx<comp::ActiveTileset>();
  if (active.entity != entt::null) {
    const auto& selection = checked_get<comp::TilesetSelection>(registry, active.entity);
    return selection.region.has_value();
  }
  else {
    return false;
  }
}

auto is_single_tile_selected_in_tileset(const entt::registry& registry) -> bool
{
  const auto& active = registry.ctx<comp::ActiveTileset>();
  if (active.entity != entt::null) {
    const auto& selection = checked_get<comp::TilesetSelection>(registry, active.entity);
    if (selection.region) {
      const auto& region = *selection.region;
      return (region.end - region.begin) == tile_position{1, 1};
    }
  }

  return false;
}

auto get_tile_to_render(const entt::registry& registry,
                        const entt::entity tilesetEntity,
                        const tile_id id) -> tile_id
{
  const auto& cache = checked_get<comp::TilesetCache>(registry, tilesetEntity);

  /* Check for already cached tile to render */
  if (const auto iter = cache.source_to_render.find(id);
      iter != cache.source_to_render.end()) {
    return iter->second;
  }

  if (const auto iter = cache.tiles.find(id); iter != cache.tiles.end()) {
    const auto entity = iter->second;

    if (const auto* animation = registry.try_get<comp::Animation>(entity)) {
      const auto frameEntity = animation->frames.at(animation->index);
      const auto& frame = checked_get<comp::AnimationFrame>(registry, frameEntity);

      /* This cache is cleared before each frame */
      cache.source_to_render[id] = frame.tile;

      return frame.tile;
    }
  }

  return id;
}

auto get_source_rect(const entt::registry& registry,
                     const entt::entity tilesetEntity,
                     const tile_id id) -> const cen::irect&
{
  const auto& cache = checked_get<comp::TilesetCache>(registry, tilesetEntity);
  return cache.source_rects.at(id);
}

auto get_tile_from_tileset(const entt::registry& registry,
                           const entt::entity entity,
                           const tile_position& position) -> tile_id
{
  const auto& tileset = checked_get<comp::Tileset>(registry, entity);

  const auto row = position.row();
  const auto col = position.col();

  if ((row >= 0) && (col >= 0) && (row < tileset.row_count) &&
      (col < tileset.column_count)) {
    const auto index = row * tileset.column_count + col;
    return tileset.first_id + tile_id{index};
  }
  else {
    return empty_tile;
  }
}

auto convert_to_local(const entt::registry& registry, const tile_id global)
    -> Maybe<tile_id>
{
  const auto entity = find_tileset_with_tile(registry, global);
  if (entity != entt::null) {
    const auto& tileset = checked_get<comp::Tileset>(registry, entity);
    return global - tileset.first_id;
  }
  else {
    return nothing;
  }
}

}  // namespace tactile::sys
