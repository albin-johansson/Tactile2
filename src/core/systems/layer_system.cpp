#include "layer_system.hpp"

#include <algorithm>  // sort
#include <cassert>    // assert
#include <utility>    // move, swap
#include <vector>     // erase

#include "core/components/group_layer.hpp"
#include "core/components/layer.hpp"
#include "core/components/parent.hpp"
#include "core/components/property.hpp"
#include "core/components/property_context.hpp"
#include "core/components/tile_layer.hpp"
#include "core/map.hpp"
#include "core/systems/tile_layer_system.hpp"

namespace Tactile::Sys {
namespace {

[[nodiscard]] auto GetNewLayerParent(const entt::registry& registry) -> entt::entity
{
  const auto active = registry.ctx<ActiveLayer>();
  if (active.entity != entt::null && registry.all_of<GroupLayer>(active.entity))
  {
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
  if (parentEntity != entt::null)
  {
    const auto& group = registry.get<GroupLayer>(parentEntity);
    return group.layers.size();
  }
  else
  {
    usize index = 0;
    for (auto&& [entity, layer, parent] : registry.view<Layer, Parent>().each())
    {
      if (layerEntity != entity && parent.entity == entt::null)
      {
        ++index;
      }
    }

    return index;
  }
}

[[nodiscard]] auto GetLayerSibling(const entt::registry& registry,
                                   const entt::entity entity,
                                   const usize targetIndex) -> entt::entity
{
  const auto& parent = registry.get<Parent>(entity);
  if (parent.entity != entt::null)
  {
    const auto& parentLayer = registry.get<GroupLayer>(parent.entity);
    for (const auto child : parentLayer.layers)
    {
      const auto& childLayer = registry.get<Layer>(child);
      if (childLayer.index == targetIndex)
      {
        return child;
      }
    }
  }
  else
  {
    for (auto&& [otherEntity, otherLayer, otherParent] :
         registry.view<Layer, Parent>().each())
    {
      if (otherParent.entity == entt::null && otherLayer.index == targetIndex)
      {
        return otherEntity;
      }
    }
  }

  return entt::null;
}

[[nodiscard]] auto CountAllLayersAbove(const entt::registry& registry,
                                       const entt::entity entity) -> usize
{
  usize count = 0;

  auto sibling = GetLayerSiblingAbove(registry, entity);
  while (sibling != entt::null)
  {
    count += GetLayerChildrenCount(registry, sibling);
    sibling = GetLayerSiblingAbove(registry, sibling);
  }

  return count;
}

void SwapLayerIndices(entt::registry& registry,
                      const entt::entity entity,
                      const entt::entity targetEntity)
{
  assert(entity != entt::null);
  assert(targetEntity != entt::null);

  auto& layer = registry.get<Layer>(entity);
  auto& targetLayer = registry.get<Layer>(targetEntity);

  std::swap(layer.index, targetLayer.index);

  SortLayers(registry);
}

void DestroyChildLayers(entt::registry& registry, const entt::entity entity)
{
  auto& group = registry.get<GroupLayer>(entity);
  for (const auto child : group.layers)
  {
    if (registry.all_of<GroupLayer>(child))
    {
      DestroyChildLayers(registry, child);
      registry.destroy(child);
    }
    else
    {
      registry.destroy(child);
    }
  }
}

}  // namespace

auto AddBasicLayer(entt::registry& registry,
                   const layer_id id,
                   const LayerType type,
                   std::string name,
                   const entt::entity parent) -> entt::entity
{
  const auto entity = registry.create();

  {
    auto& layer = registry.emplace<Layer>(entity);
    layer.id = id;
    layer.index = GetNewLayerIndex(registry, entity, parent);
    layer.type = type;
    layer.visible = true;
    layer.opacity = 1.0f;
  }

  assert(parent == entt::null || registry.all_of<GroupLayer>(parent));
  registry.emplace<Parent>(entity, parent);

  if (parent != entt::null)
  {
    auto& parentLayer = registry.get<GroupLayer>(parent);
    parentLayer.layers.push_back(entity);
  }

  auto& context = registry.emplace<PropertyContext>(entity);
  context.name = std::move(name);

  SortLayers(registry);

  return entity;
}

auto AddTileLayer(entt::registry& registry) -> entt::entity
{
  auto& map = registry.ctx<Map>();

  const auto entity = AddBasicLayer(registry,
                                    map.next_layer_id,
                                    LayerType::TileLayer,
                                    "Tile Layer",
                                    GetNewLayerParent(registry));
  ++map.next_layer_id;

  auto& tileLayer = registry.emplace<TileLayer>(entity);
  tileLayer.matrix = Sys::MakeTileMatrix(map.row_count, map.column_count);

  return entity;
}

auto AddObjectLayer(entt::registry& registry) -> entt::entity
{
  auto& map = registry.ctx<Map>();

  const auto entity = AddBasicLayer(registry,
                                    map.next_layer_id,
                                    LayerType::ObjectLayer,
                                    "Object Layer",
                                    GetNewLayerParent(registry));
  ++map.next_layer_id;

  registry.emplace<ObjectLayer>(entity);

  return entity;
}

auto AddGroupLayer(entt::registry& registry) -> entt::entity
{
  auto& map = registry.ctx<Map>();

  const auto entity = AddBasicLayer(registry,
                                    map.next_layer_id,
                                    LayerType::GroupLayer,
                                    "Group Layer",
                                    GetNewLayerParent(registry));
  ++map.next_layer_id;

  registry.emplace<GroupLayer>(entity);

  return entity;
}

void SortLayers(entt::registry& registry)
{
  auto sorter = [&](const entt::entity a, const entt::entity b) {
    const auto fst = GetLayerGlobalIndex(registry, a);
    const auto snd = GetLayerGlobalIndex(registry, b);
    return fst < snd;
  };
  registry.sort<Layer>(sorter, entt::insertion_sort{});

  /* Ensure that group layers store sorted child layers */
  for (auto&& [entity, group] : registry.view<GroupLayer>().each())
  {
    std::ranges::sort(group.layers, [&](const entt::entity a, const entt::entity b) {
      const auto& fst = registry.get<Layer>(a);
      const auto& snd = registry.get<Layer>(b);
      return fst.index < snd.index;
    });
  }
}

void RemoveLayer(entt::registry& registry, const entt::entity entity)
{
  /* Reset the active layer if we're removing the active layer. */
  auto& active = registry.ctx<ActiveLayer>();
  if (entity == active.entity)
  {
    active.entity = entt::null;
  }

  /* Fix indices of siblings that are below the removed layer */
  auto sibling = GetLayerSiblingBelow(registry, entity);
  while (sibling != entt::null)
  {
    auto& siblingLayer = registry.get<Layer>(sibling);

    assert(siblingLayer.index > 0);
    const auto newIndex = siblingLayer.index - 1;

    sibling = GetLayerSiblingBelow(registry, sibling);
    siblingLayer.index = newIndex;
  }

  /* Remove the layer from the parent group layer, if there is one. */
  const auto& parent = registry.get<Parent>(entity);
  if (parent.entity != entt::null)
  {
    auto& group = registry.get<GroupLayer>(parent.entity);
    std::erase(group.layers, entity);
  }

  if (registry.all_of<GroupLayer>(entity))
  {
    DestroyChildLayers(registry, entity);
  }

  registry.destroy(entity);
  SortLayers(registry);
}

void SelectLayer(entt::registry& registry, const entt::entity entity)
{
  assert(entity != entt::null);

  auto& active = registry.ctx<ActiveLayer>();
  active.entity = entity;
}

void MoveLayerUp(entt::registry& registry, const entt::entity entity)
{
  assert(CanMoveLayerUp(registry, entity));

  const auto targetEntity = GetLayerSiblingAbove(registry, entity);
  SwapLayerIndices(registry, entity, targetEntity);
}

void MoveLayerDown(entt::registry& registry, const entt::entity entity)
{
  assert(CanMoveLayerDown(registry, entity));

  const auto targetEntity = GetLayerSiblingBelow(registry, entity);
  SwapLayerIndices(registry, entity, targetEntity);
}

void SetLayerOpacity(entt::registry& registry,
                     const entt::entity entity,
                     const float opacity)
{
  assert(entity != entt::null);

  auto& layer = registry.get<Layer>(entity);
  layer.opacity = opacity;
}

void SetLayerVisible(entt::registry& registry,
                     const entt::entity entity,
                     const bool visible)
{
  assert(entity != entt::null);

  auto& layer = registry.get<Layer>(entity);
  layer.visible = visible;
}

auto FindLayer(const entt::registry& registry, const layer_id id) -> entt::entity
{
  for (auto&& [entity, layer] : registry.view<Layer>().each())
  {
    if (layer.id == id)
    {
      return entity;
    }
  }

  return entt::null;
}

auto GetLayerIndex(const entt::registry& registry, const entt::entity entity)
    -> usize
{
  assert(entity != entt::null);
  const auto& layer = registry.get<Layer>(entity);
  return layer.index;
}

auto GetLayerGlobalIndex(const entt::registry& registry,
                         const entt::entity sourceEntity) -> usize
{
  /* SortLayers makes use of this function, so do not assume sorted layers! */
  assert(sourceEntity != entt::null);
  const auto& sourceLayer = registry.get<Layer>(sourceEntity);
  const auto& sourceParent = registry.get<Parent>(sourceEntity);

  const auto nLayersAbove = CountAllLayersAbove(registry, sourceEntity);
  const auto base = sourceLayer.index + nLayersAbove;

  if (sourceParent.entity == entt::null)
  {
    return base;
  }
  else
  {
    return base + GetLayerGlobalIndex(registry, sourceParent.entity) + 1;
  }
}

auto GetLayerSiblingAbove(const entt::registry& registry, const entt::entity entity)
    -> entt::entity
{
  assert(entity != entt::null);
  const auto index = registry.get<Layer>(entity).index;
  return GetLayerSibling(registry, entity, index - 1);
}

auto GetLayerSiblingBelow(const entt::registry& registry, const entt::entity entity)
    -> entt::entity
{
  assert(entity != entt::null);
  const auto index = registry.get<Layer>(entity).index;
  return GetLayerSibling(registry, entity, index + 1);
}

auto GetSiblingCount(const entt::registry& registry, const entt::entity entity)
    -> usize
{
  assert(entity != entt::null);
  const auto& parent = registry.get<Parent>(entity);
  if (parent.entity == entt::null)
  {
    // The layer is root-level, so count layers without parents
    usize count = 0;

    for (auto&& [otherEntity, otherLayer, otherParent] :
         registry.view<Layer, Parent>().each())
    {
      if (otherEntity != entity && otherParent.entity == entt::null)
      {
        ++count;
      }
    }

    return count;
  }
  else
  {
    const auto& parentLayer = registry.get<GroupLayer>(parent.entity);
    return parentLayer.layers.size() - 1;  // Exclude the queried layer
  }
}

auto GetLayerChildrenCount(const entt::registry& registry, const entt::entity entity)
    -> usize
{
  assert(entity != entt::null);
  if (const auto* group = registry.try_get<GroupLayer>(entity))
  {
    usize count = group->layers.size();

    for (const auto child : group->layers)
    {
      count += GetLayerChildrenCount(registry, child);
    }

    return count;
  }
  else
  {
    return 0;
  }
}

auto GetLayerOpacity(const entt::registry& registry, const entt::entity entity)
    -> float
{
  assert(entity != entt::null);
  const auto& layer = registry.get<Layer>(entity);
  return layer.opacity;
}

auto IsLayerVisible(const entt::registry& registry, const entt::entity entity)
    -> bool
{
  assert(entity != entt::null);
  const auto& layer = registry.get<Layer>(entity);
  return layer.visible;
}

auto CanMoveLayerUp(const entt::registry& registry, const entt::entity entity)
    -> bool
{
  assert(entity != entt::null);
  return registry.get<Layer>(entity).index > 0;
}

auto CanMoveLayerDown(const entt::registry& registry, const entt::entity entity)
    -> bool
{
  assert(entity != entt::null);
  const auto index = registry.get<Layer>(entity).index;
  const auto nSiblings = GetSiblingCount(registry, entity);
  return index < nSiblings;
}

}  // namespace Tactile::Sys
