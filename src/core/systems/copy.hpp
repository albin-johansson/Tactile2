#pragma once

#include <entt.hpp>  // registry, entity

#include "core/components/group_layer.hpp"
#include "core/components/layer.hpp"
#include "core/components/object.hpp"
#include "core/components/object_layer.hpp"
#include "core/components/parent.hpp"
#include "core/components/property.hpp"
#include "core/components/property_context.hpp"
#include "core/map.hpp"

namespace Tactile::Sys {

// TODO rename to something like "Duplicate"
template <typename T>
auto Copy(entt::registry& registry,
          const entt::entity source,
          const entt::entity destination) -> T&
{
  return registry.emplace<T>(destination, registry.get<T>(source));
}

template <>
inline auto Copy<PropertyContext>(entt::registry& registry,
                                  const entt::entity source,
                                  const entt::entity destination) -> PropertyContext&
{
  auto& context = registry.emplace<PropertyContext>(destination);

  const auto& srcContext = registry.get<PropertyContext>(source);
  context.name = srcContext.name;

  for (const auto srcPropertyEntity : srcContext.properties)
  {
    const auto propertyEntity = registry.create();
    context.properties.push_back(propertyEntity);

    Copy<Property>(registry, srcPropertyEntity, propertyEntity);
  }

  return context;
}

template <>
inline auto Copy<ObjectLayer>(entt::registry& registry,
                              const entt::entity source,
                              const entt::entity destination) -> ObjectLayer&
{
  auto& map = registry.ctx<Map>();
  auto& layer = registry.emplace<ObjectLayer>(destination);

  const auto& sourceLayer = registry.get<ObjectLayer>(source);
  for (const auto sourceObject : sourceLayer.objects)
  {
    const auto objectEntity = registry.create();
    layer.objects.push_back(objectEntity);

    Copy<PropertyContext>(registry, sourceObject, objectEntity);

    auto& object = Copy<Object>(registry, sourceObject, objectEntity);
    object.id = map.next_object_id;
    ++map.next_object_id;
  }

  return layer;
}

auto DuplicateLayer(entt::registry& registry,
                    entt::entity entity,
                    entt::entity parent,
                    bool recursive) -> entt::entity;

template <>
inline auto Copy<GroupLayer>(entt::registry& registry,
                             const entt::entity source,
                             const entt::entity destination) -> GroupLayer&
{
  auto& map = registry.ctx<Map>();

  auto& group = registry.emplace<GroupLayer>(destination);
  for (const auto sourceChild : registry.get<GroupLayer>(source).layers)
  {
    /* We don't need to add the created child layer to the group layer explicitly */
    DuplicateLayer(registry, sourceChild, destination, true);
  }

  return group;
}

}  // namespace Tactile::Sys
