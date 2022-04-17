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

#include "layer_system.hpp"

#include <algorithm>  // sort
#include <utility>    // move, swap
#include <vector>     // erase

#include <entt/entity/registry.hpp>
#include <fmt/format.h>

#include "core/components/attributes.hpp"
#include "core/components/objects.hpp"
#include "core/components/parent.hpp"
#include "core/map_info.hpp"
#include "core/systems/duplicate_comp.hpp"
#include "core/systems/layers/layer_tree_system.hpp"
#include "core/systems/layers/tile_layer_system.hpp"
#include "core/systems/property_system.hpp"
#include "core/systems/registry_system.hpp"
#include "core/utils/tiles.hpp"
#include "misc/assert.hpp"

namespace tactile::sys {
namespace {

[[nodiscard]] auto _new_layer_parent(const entt::registry& registry) -> entt::entity
{
  const auto active = registry.ctx<comp::ActiveLayer>();
  if (active.entity != entt::null && registry.all_of<comp::GroupLayer>(active.entity)) {
    return active.entity;
  }
  {
    return entt::null;
  }
}

[[nodiscard]] auto _new_layer_index(const entt::registry& registry,
                                    const entt::entity layerEntity,
                                    const entt::entity parentEntity) -> usize
{
  if (parentEntity != entt::null) {
    const auto& node = checked_get<comp::LayerTreeNode>(registry, parentEntity);
    return node.children.size();
  }
  else {
    usize index = 0;
    for (auto&& [entity, layer, parent] :
         registry.view<comp::Layer, comp::Parent>().each()) {
      if (layerEntity != entity && parent.entity == entt::null) {
        ++index;
      }
    }

    return index;
  }
}

[[nodiscard]] auto _copy_layer(const entt::registry& registry, const entt::entity source)
    -> LayerSnapshot
{
  TACTILE_ASSERT(source != entt::null);
  TACTILE_ASSERT(registry.all_of<comp::Layer>(source));
  TACTILE_ASSERT(registry.all_of<comp::LayerTreeNode>(source));
  TACTILE_ASSERT(registry.all_of<comp::Parent>(source));
  assert((registry.any_of<comp::TileLayer, comp::ObjectLayer, comp::GroupLayer>(source)));

  LayerSnapshot snapshot;
  snapshot.index = checked_get<comp::LayerTreeNode>(registry, source).index;
  snapshot.core = checked_get<comp::Layer>(registry, source);
  snapshot.context = copy_attribute_context(registry, source);

  const auto parentEntity = checked_get<comp::Parent>(registry, source).entity;
  if (parentEntity != entt::null) {
    snapshot.parent = checked_get<comp::Layer>(registry, parentEntity).id;
  }

  switch (snapshot.core.type) {
    case LayerType::TileLayer: {
      snapshot.tiles = checked_get<comp::TileLayer>(registry, source).matrix;
      break;
    }
    case LayerType::ObjectLayer: {
      auto& objects = snapshot.objects.emplace();

      for (const auto objectEntity :
           checked_get<comp::ObjectLayer>(registry, source).objects) {
        auto& objectSnapshot = objects.emplace_back();
        objectSnapshot.core = checked_get<comp::Object>(registry, objectEntity);
        objectSnapshot.context = copy_attribute_context(registry, objectEntity);
      }

      break;
    }
    case LayerType::GroupLayer: {
      auto& children = snapshot.children.emplace();

      for (const auto child :
           checked_get<comp::LayerTreeNode>(registry, source).children) {
        children.push_back(_copy_layer(registry, child));
      }

      break;
    }
  }

  return snapshot;
}

void _restore_layer_index(entt::registry& registry,
                          const entt::entity layerEntity,
                          const usize previousIndex)
{
  while (layer_local_index(registry, layerEntity) != previousIndex) {
    const auto index = layer_local_index(registry, layerEntity);
    if (index > previousIndex) {
      move_layer_up(registry, layerEntity);
    }
    else if (index < previousIndex) {
      move_layer_down(registry, layerEntity);
    }
  }
}

}  // namespace

auto make_basic_layer(entt::registry& registry,
                      const LayerID id,
                      const LayerType type,
                      std::string name,
                      const entt::entity parent) -> entt::entity
{
  const auto entity = registry.create();

  {
    auto& node = registry.emplace<comp::LayerTreeNode>(entity);
    node.index = _new_layer_index(registry, entity, parent);
  }

  {
    auto& layer = registry.emplace<comp::Layer>(entity);
    layer.id = id;
    layer.type = type;
    layer.visible = true;
    layer.opacity = 1.0f;
  }

  TACTILE_ASSERT(parent == entt::null || registry.all_of<comp::GroupLayer>(parent));
  registry.emplace<comp::Parent>(entity, parent);

  if (parent != entt::null) {
    TACTILE_ASSERT(registry.all_of<comp::LayerTreeNode>(parent));
    auto& parentNode = checked_get<comp::LayerTreeNode>(registry, parent);
    parentNode.children.push_back(entity);
  }

  {
    auto& context = add_attribute_context(registry, entity);
    context.name = std::move(name);
  }

  return entity;
}

auto make_tile_layer(entt::registry& registry) -> entt::entity
{
  auto& map = registry.ctx<MapInfo>();

  const auto entity =
      make_basic_layer(registry,
                       map.next_layer_id,
                       LayerType::TileLayer,
                       fmt::format("Tile Layer {}", map.tile_layer_suffix),
                       _new_layer_parent(registry));
  ++map.next_layer_id;
  ++map.tile_layer_suffix;

  auto& tileLayer = registry.emplace<comp::TileLayer>(entity);
  tileLayer.matrix = make_tile_matrix(map.row_count, map.column_count);

  return entity;
}

auto make_object_layer(entt::registry& registry) -> entt::entity
{
  auto& map = registry.ctx<MapInfo>();

  const auto entity =
      make_basic_layer(registry,
                       map.next_layer_id,
                       LayerType::ObjectLayer,
                       fmt::format("Object Layer {}", map.object_layer_suffix),
                       _new_layer_parent(registry));
  ++map.next_layer_id;
  ++map.object_layer_suffix;

  registry.emplace<comp::ObjectLayer>(entity);

  return entity;
}

auto make_group_layer(entt::registry& registry) -> entt::entity
{
  auto& map = registry.ctx<MapInfo>();

  const auto entity =
      make_basic_layer(registry,
                       map.next_layer_id,
                       LayerType::GroupLayer,
                       fmt::format("Group Layer {}", map.group_layer_suffix),
                       _new_layer_parent(registry));
  ++map.next_layer_id;
  ++map.group_layer_suffix;

  registry.emplace<comp::GroupLayer>(entity);

  return entity;
}

auto remove_layer(entt::registry& registry, const entt::entity entity) -> LayerSnapshot
{
  auto snapshot = _copy_layer(registry, entity);

  auto maybe_reset = [&](entt::entity& active, const entt::entity entity) {
    if (active != entt::null) {
      if (entity == active || is_child_layer_node(registry, entity, active)) {
        active = entt::null;
      }
    }
  };

  maybe_reset(registry.ctx<comp::ActiveLayer>().entity, entity);
  maybe_reset(registry.ctx<comp::ActiveAttributeContext>().entity, entity);

  destroy_layer_node(registry, entity);

  return snapshot;
}

auto restore_layer(entt::registry& registry, LayerSnapshot snapshot) -> entt::entity
{
  entt::entity parent{entt::null};
  if (snapshot.parent.has_value()) {
    parent = find_layer(registry, *snapshot.parent);
  }

  const auto entity = make_basic_layer(registry,
                                       snapshot.core.id,
                                       snapshot.core.type,
                                       snapshot.context.name,
                                       parent);

  {
    auto& layer = checked_get<comp::Layer>(registry, entity);
    layer.opacity = snapshot.core.opacity;
    layer.visible = snapshot.core.visible;
  }

  restore_attribute_context(registry, entity, std::move(snapshot.context));

  switch (snapshot.core.type) {
    case LayerType::TileLayer: {
      auto& tileLayer = registry.emplace<comp::TileLayer>(entity);
      tileLayer.matrix = snapshot.tiles.value();
      break;
    }
    case LayerType::ObjectLayer: {
      auto& objectLayer = registry.emplace<comp::ObjectLayer>(entity);
      for (auto objectSnapshot : snapshot.objects.value()) {
        const auto objectEntity = registry.create();

        registry.emplace<comp::Object>(objectEntity, std::move(objectSnapshot.core));
        restore_attribute_context(registry,
                                  objectEntity,
                                  std::move(objectSnapshot.context));

        objectLayer.objects.push_back(objectEntity);
      }
      break;
    }
    case LayerType::GroupLayer: {
      /* We don't need to add the children to the restored group here, that is
         handled by make_basic_layer. */
      registry.emplace<comp::GroupLayer>(entity);
      for (auto layerSnapshot : snapshot.children.value()) {
        restore_layer(registry, std::move(layerSnapshot));
      }
      break;
    }
  }

  _restore_layer_index(registry, entity, snapshot.index);

  sort_layers(registry);
  return entity;
}

auto duplicate_layer(entt::registry& registry, const entt::entity source) -> entt::entity
{
  const auto& sourceParent = checked_get<comp::Parent>(registry, source);
  const auto copy = duplicate_layer(registry, source, sourceParent.entity, false);

  sort_layers(registry);

  return copy;
}

auto duplicate_layer(entt::registry& registry,
                     const entt::entity source,
                     const entt::entity parent,
                     const bool recursive) -> entt::entity
{
  /* The recursive flag determines whether indices need to be adjusted, since we
     do not touch indices of children of the original source layer that we want to
     duplicate. */

  if (!recursive) {
    increment_layer_indices_of_siblings_below(registry, source);
  }

  const auto copy = registry.create();

  {
    auto& node = registry.emplace<comp::LayerTreeNode>(copy);
    node.index = checked_get<comp::LayerTreeNode>(registry, source).index;
  }

  registry.emplace<comp::Parent>(copy, parent);

  if (parent != entt::null) {
    auto& parentNode = checked_get<comp::LayerTreeNode>(registry, parent);
    parentNode.children.push_back(copy);
  }

  {
    auto& context = DuplicateComp<comp::AttributeContext>(registry, source, copy);
    if (!recursive) {
      context.name += " (Copy)";
    }
  }

  {
    auto& map = registry.ctx<MapInfo>();
    auto& layer = DuplicateComp<comp::Layer>(registry, source, copy);
    layer.id = map.next_layer_id;

    if (!recursive) {
      const auto sourceNode = checked_get<comp::LayerTreeNode>(registry, source);

      auto& node = checked_get<comp::LayerTreeNode>(registry, copy);
      node.index = sourceNode.index + 1u;
    }

    ++map.next_layer_id;
  }

  if (registry.all_of<comp::TileLayer>(source)) {
    DuplicateComp<comp::TileLayer>(registry, source, copy);
  }
  else if (registry.all_of<comp::ObjectLayer>(source)) {
    DuplicateComp<comp::ObjectLayer>(registry, source, copy);
  }
  else if (registry.all_of<comp::GroupLayer>(source)) {
    registry.emplace<comp::GroupLayer>(copy);
    for (const auto sourceChild :
         checked_get<comp::LayerTreeNode>(registry, source).children) {
      /* We don't need to add the created child layer to the group layer explicitly */
      duplicate_layer(registry, sourceChild, copy, true);
    }
  }

  return copy;
}

void select_layer(entt::registry& registry, const LayerID id)
{
  const auto& [layerEntity, layer] = sys::get_layer(registry, id);
  auto& active = registry.ctx<comp::ActiveLayer>();
  active.entity = layerEntity;
}

auto find_layer(const entt::registry& registry, const LayerID id) -> entt::entity
{
  for (auto&& [entity, layer] : registry.view<comp::Layer>().each()) {
    if (layer.id == id) {
      return entity;
    }
  }

  return entt::null;
}

auto get_layer_entity(const entt::registry& registry, const LayerID id) -> entt::entity
{
  const auto entity = find_layer(registry, id);
  if (entity != entt::null && registry.all_of<comp::Layer>(entity)) {
    return entity;
  }
  else {
    panic("Invalid layer ID!");
  }
}

auto get_layer(entt::registry& registry, const LayerID id)
    -> std::pair<entt::entity, comp::Layer&>
{
  const auto entity = find_layer(registry, id);
  if (entity != entt::null && registry.all_of<comp::Layer>(entity)) {
    return {entity, checked_get<comp::Layer>(registry, entity)};
  }
  else {
    panic("Invalid layer identifier!");
  }
}

auto get_layer(const entt::registry& registry, const LayerID id)
    -> std::pair<entt::entity, const comp::Layer&>
{
  const auto entity = find_layer(registry, id);
  if (entity != entt::null && registry.all_of<comp::Layer>(entity)) {
    return {entity, checked_get<comp::Layer>(registry, entity)};
  }
  else {
    panic("Invalid layer identifier!");
  }
}

auto get_active_layer(const entt::registry& registry) -> entt::entity
{
  const auto& active = registry.ctx<comp::ActiveLayer>();
  return active.entity;
}

auto is_tile_layer_active(const entt::registry& registry) -> bool
{
  const auto& active = registry.ctx<comp::ActiveLayer>();
  if (active.entity != entt::null) {
    return registry.all_of<comp::TileLayer>(active.entity);
  }
  else {
    return false;
  }
}

auto is_object_layer_active(const entt::registry& registry) -> bool
{
  const auto& active = registry.ctx<comp::ActiveLayer>();
  if (active.entity != entt::null) {
    return registry.all_of<comp::ObjectLayer>(active.entity);
  }
  else {
    return false;
  }
}

auto get_active_layer_id(const entt::registry& registry) -> std::optional<LayerID>
{
  const auto& active = registry.ctx<comp::ActiveLayer>();
  if (active.entity != entt::null) {
    return checked_get<comp::Layer>(registry, active.entity).id;
  }
  else {
    return std::nullopt;
  }
}

}  // namespace tactile::sys
