#include "layer_system.hpp"

#include <algorithm>  // sort
#include <cassert>    // assert
#include <format>     // format
#include <utility>    // move, swap
#include <vector>     // erase

#include "core/components/group_layer.hpp"
#include "core/components/layer.hpp"
#include "core/components/layer_tree_node.hpp"
#include "core/components/object.hpp"
#include "core/components/object_layer.hpp"
#include "core/components/parent.hpp"
#include "core/components/property.hpp"
#include "core/components/property_context.hpp"
#include "core/components/tile_layer.hpp"
#include "core/ctx/map.hpp"
#include "core/systems/duplicate_comp.hpp"
#include "core/systems/property_system.hpp"
#include "layer_tree_system.hpp"
#include "tile_layer_system.hpp"

namespace Tactile::Sys {
namespace {

[[nodiscard]] auto GetNewLayerParent(const entt::registry& registry) -> entt::entity
{
  const auto active = registry.ctx<ActiveLayer>();
  if (active.entity != entt::null && registry.all_of<GroupLayer>(active.entity)) {
    return active.entity;
  }
  {
    return entt::null;
  }
}

[[nodiscard]] auto GetNewLayerIndex(const entt::registry& registry,
                                    const entt::entity layerEntity,
                                    const entt::entity parentEntity) -> usize
{
  if (parentEntity != entt::null) {
    const auto& node = registry.get<LayerTreeNode>(parentEntity);
    return node.children.size();
  }
  else {
    usize index = 0;
    for (auto&& [entity, layer, parent] : registry.view<Layer, Parent>().each()) {
      if (layerEntity != entity && parent.entity == entt::null) {
        ++index;
      }
    }

    return index;
  }
}

}  // namespace

auto AddBasicLayer(entt::registry& registry,
                   const LayerID id,
                   const LayerType type,
                   std::string name,
                   const entt::entity parent) -> entt::entity
{
  const auto entity = registry.create();

  {
    auto& node = registry.emplace<LayerTreeNode>(entity);
    node.index = GetNewLayerIndex(registry, entity, parent);
  }

  {
    auto& layer = registry.emplace<Layer>(entity);
    layer.id = id;
    layer.type = type;
    layer.visible = true;
    layer.opacity = 1.0f;
  }

  assert(parent == entt::null || registry.all_of<GroupLayer>(parent));
  registry.emplace<Parent>(entity, parent);

  if (parent != entt::null) {
    assert(registry.all_of<LayerTreeNode>(parent));
    auto& parentNode = registry.get<LayerTreeNode>(parent);
    parentNode.children.push_back(entity);
  }

  {
    auto& context = Sys::AddPropertyContext(registry, entity);
    context.name = std::move(name);
  }

  return entity;
}

auto AddTileLayer(entt::registry& registry) -> entt::entity
{
  auto& map = registry.ctx<Map>();

  const auto entity = AddBasicLayer(registry,
                                    map.next_layer_id,
                                    LayerType::TileLayer,
                                    std::format("Tile Layer {}", map.tile_layer_suffix),
                                    GetNewLayerParent(registry));
  ++map.next_layer_id;
  ++map.tile_layer_suffix;

  auto& tileLayer = registry.emplace<TileLayer>(entity);
  tileLayer.matrix = MakeTileMatrix(map.row_count, map.column_count);

  return entity;
}

auto AddObjectLayer(entt::registry& registry) -> entt::entity
{
  auto& map = registry.ctx<Map>();

  const auto entity =
      AddBasicLayer(registry,
                    map.next_layer_id,
                    LayerType::ObjectLayer,
                    std::format("Object Layer {}", map.object_layer_suffix),
                    GetNewLayerParent(registry));
  ++map.next_layer_id;
  ++map.object_layer_suffix;

  registry.emplace<ObjectLayer>(entity);

  return entity;
}

auto AddGroupLayer(entt::registry& registry) -> entt::entity
{
  auto& map = registry.ctx<Map>();

  const auto entity = AddBasicLayer(registry,
                                    map.next_layer_id,
                                    LayerType::GroupLayer,
                                    std::format("Group Layer {}", map.group_layer_suffix),
                                    GetNewLayerParent(registry));
  ++map.next_layer_id;
  ++map.group_layer_suffix;

  registry.emplace<GroupLayer>(entity);

  return entity;
}

auto RestoreLayer(entt::registry& registry, LayerSnapshot snapshot) -> entt::entity
{
  entt::entity parent{entt::null};
  if (snapshot.parent.has_value()) {
    parent = FindLayer(registry, *snapshot.parent);
  }

  const auto entity = AddBasicLayer(registry,
                                    snapshot.core.id,
                                    snapshot.core.type,
                                    snapshot.context.name,
                                    parent);

  {
    auto& layer = registry.get<Layer>(entity);
    layer.opacity = snapshot.core.opacity;
    layer.visible = snapshot.core.visible;
  }

  RestorePropertyContext(registry, entity, std::move(snapshot.context));

  switch (snapshot.core.type) {
    case LayerType::TileLayer: {
      auto& tileLayer = registry.emplace<TileLayer>(entity);
      tileLayer.matrix = snapshot.tiles.value();
      break;
    }
    case LayerType::ObjectLayer: {
      auto& objectLayer = registry.emplace<ObjectLayer>(entity);
      for (auto objectSnapshot : snapshot.objects.value()) {
        const auto objectEntity = registry.create();

        registry.emplace<Object>(objectEntity, std::move(objectSnapshot.core));
        RestorePropertyContext(registry, objectEntity, std::move(objectSnapshot.context));

        objectLayer.objects.push_back(objectEntity);
      }
      break;
    }
    case LayerType::GroupLayer: {
      /* We don't need to add the children to the restored group here, that is
         handled by AddBasicLayer. */
      registry.emplace<GroupLayer>(entity);
      for (auto layerSnapshot : snapshot.children.value()) {
        RestoreLayer(registry, std::move(layerSnapshot));
      }
      break;
    }
  }

  /* Restore the previous layer index */
  while (GetLayerIndex(registry, entity) != snapshot.index) {
    const auto index = GetLayerIndex(registry, entity);
    if (index > snapshot.index) {
      LayerTree::MoveNodeUp(registry, entity);
    }
    else if (index < snapshot.index) {
      LayerTree::MoveNodeDown(registry, entity);
    }
  }

  LayerTree::SortNodes(registry);
  return entity;
}

void SortLayers(entt::registry& registry)
{
  LayerTree::SortNodes(registry);
}

void RemoveLayer(entt::registry& registry, const entt::entity entity)
{
  auto maybe_reset = [&](entt::entity& active, const entt::entity entity) {
    if (active != entt::null) {
      if (entity == active || LayerTree::IsChildNode(registry, entity, active)) {
        active = entt::null;
      }
    }
  };

  maybe_reset(registry.ctx<ActiveLayer>().entity, entity);
  maybe_reset(registry.ctx<ActivePropertyContext>().entity, entity);

  LayerTree::DestroyNode(registry, entity);
}

void SelectLayer(entt::registry& registry, const entt::entity entity)
{
  assert(entity != entt::null);

  auto& active = registry.ctx<ActiveLayer>();
  active.entity = entity;
}

auto CopyLayer(const entt::registry& registry, const entt::entity source) -> LayerSnapshot
{
  assert(source != entt::null);
  assert(registry.all_of<Layer>(source));
  assert(registry.all_of<LayerTreeNode>(source));
  assert(registry.all_of<Parent>(source));
  assert((registry.any_of<TileLayer, ObjectLayer, GroupLayer>(source)));

  LayerSnapshot snapshot;
  snapshot.index = registry.get<LayerTreeNode>(source).index;
  snapshot.core = registry.get<Layer>(source);
  snapshot.context = CopyPropertyContext(registry, source);

  const auto parentEntity = registry.get<Parent>(source).entity;
  if (parentEntity != entt::null) {
    snapshot.parent = registry.get<Layer>(parentEntity).id;
  }

  switch (snapshot.core.type) {
    case LayerType::TileLayer: {
      snapshot.tiles = registry.get<TileLayer>(source).matrix;
      break;
    }
    case LayerType::ObjectLayer: {
      auto& objects = snapshot.objects.emplace();

      for (const auto objectEntity : registry.get<ObjectLayer>(source).objects) {
        auto& objectSnapshot = objects.emplace_back();
        objectSnapshot.core = registry.get<Object>(objectEntity);
        objectSnapshot.context = CopyPropertyContext(registry, objectEntity);
      }

      break;
    }
    case LayerType::GroupLayer: {
      auto& children = snapshot.children.emplace();

      for (const auto child : registry.get<LayerTreeNode>(source).children) {
        children.push_back(CopyLayer(registry, child));
      }

      break;
    }
  }

  return snapshot;
}

auto DuplicateLayer(entt::registry& registry, const entt::entity source) -> entt::entity
{
  const auto& sourceParent = registry.get<Parent>(source);
  const auto copy = DuplicateLayer(registry, source, sourceParent.entity, false);

  LayerTree::SortNodes(registry);

  return copy;
}

auto DuplicateLayer(entt::registry& registry,
                    const entt::entity source,
                    const entt::entity parent,
                    const bool recursive) -> entt::entity
{
  /* The recursive flag determines whether indices need to be adjusted, since we
     do not touch indices of children of the original source layer that we want to
     duplicate. */

  if (!recursive) {
    LayerTree::IncrementIndicesOfSiblingsBelow(registry, source);
  }

  const auto copy = registry.create();

  {
    auto& node = registry.emplace<LayerTreeNode>(copy);
    node.index = registry.get<LayerTreeNode>(source).index;
  }

  registry.emplace<Parent>(copy, parent);

  if (parent != entt::null) {
    auto& parentNode = registry.get<LayerTreeNode>(parent);
    parentNode.children.push_back(copy);
  }

  {
    auto& context = DuplicateComp<PropertyContext>(registry, source, copy);
    if (!recursive) {
      context.name += " (Copy)";
    }
  }

  {
    auto& map = registry.ctx<Map>();
    auto& layer = DuplicateComp<Layer>(registry, source, copy);
    layer.id = map.next_layer_id;

    if (!recursive) {
      const auto sourceNode = registry.get<LayerTreeNode>(source);

      auto& node = registry.get<LayerTreeNode>(copy);
      node.index = sourceNode.index + 1u;
    }

    ++map.next_layer_id;
  }

  if (registry.all_of<TileLayer>(source)) {
    DuplicateComp<TileLayer>(registry, source, copy);
  }
  else if (registry.all_of<ObjectLayer>(source)) {
    DuplicateComp<ObjectLayer>(registry, source, copy);
  }
  else if (registry.all_of<GroupLayer>(source)) {
    for (const auto sourceChild : registry.get<LayerTreeNode>(source).children) {
      /* We don't need to add the created child layer to the group layer explicitly */
      DuplicateLayer(registry, sourceChild, copy, true);
    }
  }

  return copy;
}

void MoveLayerUp(entt::registry& registry, const entt::entity entity)
{
  LayerTree::MoveNodeUp(registry, entity);
}

void MoveLayerDown(entt::registry& registry, const entt::entity entity)
{
  LayerTree::MoveNodeDown(registry, entity);
}

void SetLayerOpacity(entt::registry& registry,
                     const entt::entity entity,
                     const float opacity)
{
  assert(entity != entt::null);
  assert(registry.all_of<Layer>(entity));

  auto& layer = registry.get<Layer>(entity);
  layer.opacity = opacity;
}

void SetLayerVisible(entt::registry& registry,
                     const entt::entity entity,
                     const bool visible)
{
  assert(entity != entt::null);
  assert(registry.all_of<Layer>(entity));

  auto& layer = registry.get<Layer>(entity);
  layer.visible = visible;
}

auto FindLayer(const entt::registry& registry, const LayerID id) -> entt::entity
{
  for (auto&& [entity, layer] : registry.view<Layer>().each()) {
    if (layer.id == id) {
      return entity;
    }
  }

  return entt::null;
}

auto GetActiveLayer(const entt::registry& registry) -> entt::entity
{
  const auto& active = registry.ctx<ActiveLayer>();
  return active.entity;
}

auto GetLayerIndex(const entt::registry& registry, const entt::entity entity) -> usize
{
  assert(entity != entt::null);
  assert(registry.all_of<LayerTreeNode>(entity));
  return registry.get<LayerTreeNode>(entity).index;
}

auto GetLayerOpacity(const entt::registry& registry, const entt::entity entity) -> float
{
  assert(entity != entt::null);
  assert(registry.all_of<Layer>(entity));
  return registry.get<Layer>(entity).opacity;
}

auto GetLayerId(const entt::registry& registry, const entt::entity entity) -> LayerID
{
  assert(entity != entt::null);
  assert(registry.all_of<Layer>(entity));
  return registry.get<Layer>(entity).id;
}

auto IsLayerVisible(const entt::registry& registry, const entt::entity entity) -> bool
{
  assert(entity != entt::null);
  assert(registry.all_of<Layer>(entity));
  return registry.get<Layer>(entity).visible;
}

auto CanMoveLayerUp(const entt::registry& registry, const entt::entity entity) -> bool
{
  return LayerTree::CanMoveNodeUp(registry, entity);
}

auto CanMoveLayerDown(const entt::registry& registry, const entt::entity entity) -> bool
{
  return LayerTree::CanMoveNodeDown(registry, entity);
}

auto IsTileLayerActive(const entt::registry& registry) -> bool
{
  const auto& active = registry.ctx<ActiveLayer>();
  if (active.entity != entt::null) {
    return registry.all_of<TileLayer>(active.entity);
  }
  else {
    return false;
  }
}

auto GetActiveLayerID(const entt::registry& registry) -> Maybe<LayerID>
{
  const auto& active = registry.ctx<ActiveLayer>();
  if (active.entity != entt::null) {
    return registry.get<Layer>(active.entity).id;
  }
  else {
    return nothing;
  }
}

}  // namespace Tactile::Sys
