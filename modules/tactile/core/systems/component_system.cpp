#include "component_system.hpp"

#include <utility>  // move

#include <fmt/format.h>
#include <tactile_stdlib.hpp>

#include "assert.hpp"
#include "core/components/property_context.hpp"
#include "logging.hpp"
#include "property_system.hpp"
#include "throw.hpp"

namespace Tactile::Sys {
namespace {

[[nodiscard]] auto GetComponentAttribute(const entt::registry& registry,
                                         const ComponentID id,
                                         const std::string_view attribute)
    -> decltype(ComponentDef::attributes)::const_iterator
{
  const auto [defEntity, def] = GetComponentDef(registry, id);
  if (const auto it = def.attributes.find(attribute); it != def.attributes.end()) {
    return it;
  }
  else {
    ThrowTraced(TactileError{"Invalid component attribute name!"});
  }
}

[[nodiscard]] auto GetComponentAttribute(entt::registry& registry,
                                         const ComponentID id,
                                         const std::string_view attribute)
    -> decltype(ComponentDef::attributes)::iterator
{
  const auto [defEntity, def] = GetComponentDef(registry, id);
  if (const auto it = def.attributes.find(attribute); it != def.attributes.end()) {
    return it;
  }
  else {
    ThrowTraced(TactileError{"Invalid component attribute name!"});
  }
}

[[nodiscard]] auto GetComponentValue(Component& component,
                                     const std::string_view attribute)
    -> decltype(Component::values)::iterator
{
  if (const auto it = component.values.find(attribute); it != component.values.end()) {
    return it;
  }
  else {
    ThrowTraced(TactileError{"Invalid component attribute name!"});
  }
}

[[nodiscard]] auto GetComponent(entt::registry& registry,
                                const ContextID contextId,
                                const ComponentID componentId) -> Component&
{
  auto& context = GetContext(registry, contextId);

  for (const auto componentEntity : context.components) {
    auto& component = registry.get<Component>(componentEntity);
    if (component.type == componentId) {
      return component;
    }
  }

  ThrowTraced(TactileError{"Did not find component!"});
}

}  // namespace

void CreateComponentDef(entt::registry& registry, const ComponentID id, std::string name)
{
  TACTILE_ASSERT(!IsComponentValid(registry, id));
  TACTILE_ASSERT(!IsComponentNameTaken(registry, name));

  LogDebug("Creating component definition with explicit ID '{}'", id);

  const auto entity = registry.create();

  auto& def = registry.emplace<ComponentDef>(entity);
  def.id = id;
  def.name = std::move(name);
}

auto CreateComponentDef(entt::registry& registry, std::string name) -> ComponentID
{
  static ComponentID next_id = 1;

  TACTILE_ASSERT(!IsComponentNameTaken(registry, name));
  TACTILE_ASSERT_MSG(!IsComponentValid(registry, next_id),
                     "Verify use of use explicit component identifiers!");

  LogDebug("Creating component definition '{}' (ID '{}')", name, next_id);

  const auto entity = registry.create();

  auto& def = registry.emplace<ComponentDef>(entity);
  def.id = next_id;
  def.name = std::move(name);

  ++next_id;

  return def.id;
}

auto RemoveComponentDef(entt::registry& registry, const ComponentID id)
    -> RemoveComponentDefResult
{
  LogDebug("Deleting component definition '{}'", id);

  const auto [defEntity, def] = GetComponentDef(registry, id);

  RemoveComponentDefResult result;
  result.name = def.name;
  result.attributes = def.attributes;

  /* Cache attributes from components that will be removed */
  auto cache = [&](const ComponentID componentId, const PropertyContext& context) {
    for (const auto componentEntity : context.components) {
      const auto& component = registry.get<Component>(componentEntity);
      if (component.type == componentId) {
        result.values[context.id] = component.values;
      }
    }
  };

  /* Removes component entities of the specified type */
  auto remove = [&, id](std::vector<entt::entity>& components) {
    std::erase_if(components, [&](const entt::entity componentEntity) {
      const auto& component = registry.get<Component>(componentEntity);
      return component.type == id;
    });
  };

  auto& root = registry.ctx<PropertyContext>();
  cache(def.id, root);
  remove(root.components);

  for (auto&& [entity, context] : registry.view<PropertyContext>().each()) {
    cache(def.id, context);
    remove(context.components);
  }

  registry.destroy(defEntity);

  return result;
}

void RestoreComponentDef(entt::registry& registry, RemoveComponentDefResult snapshot)
{
  LogDebug("Restoring component definition '{}'", snapshot.id);

  CreateComponentDef(registry, snapshot.id, snapshot.name);
  for (auto&& [name, value] : snapshot.attributes) {
    CreateComponentAttribute(registry, snapshot.id, name, value);
  }

  for (auto&& [contextId, values] : snapshot.values) {
    LogVerbose("Restoring component '{}' for context '{}'", snapshot.id, contextId);

    auto& context = Sys::GetContext(registry, contextId);

    const auto componentEntity = registry.create();
    auto& component = registry.emplace<Component>(componentEntity);
    component.type = snapshot.id;
    component.values = std::move(values);

    context.components.push_back(componentEntity);
  }
}

void RenameComponentDef(entt::registry& registry, const ComponentID id, std::string name)
{
  TACTILE_ASSERT(!Sys::IsComponentNameTaken(registry, name));

  LogDebug("Renaming component definition '{}' to '{}'", id, name);

  auto [entity, def] = GetComponentDef(registry, id);
  def.name = std::move(name);
}

auto FindComponentDef(const entt::registry& registry, const ComponentID id)
    -> entt::entity
{
  for (auto&& [entity, component] : registry.view<ComponentDef>().each()) {
    if (component.id == id) {
      return entity;
    }
  }

  return entt::null;
}

auto GetComponentDef(entt::registry& registry, const ComponentID id)
    -> std::pair<entt::entity, ComponentDef&>
{
  const auto entity = FindComponentDef(registry, id);
  if (entity != entt::null) {
    return {entity, registry.get<ComponentDef>(entity)};
  }
  else {
    ThrowTraced(TactileError{"Failed to find component definition with specified ID!"});
  }
}

auto GetComponentDef(const entt::registry& registry, const ComponentID id)
    -> std::pair<entt::entity, const ComponentDef&>
{
  const auto entity = FindComponentDef(registry, id);
  if (entity != entt::null) {
    return {entity, registry.get<ComponentDef>(entity)};
  }
  else {
    ThrowTraced(TactileError{"Failed to find component definition with specified ID!"});
  }
}

auto GetComponentDef(const entt::registry& registry, const std::string_view name)
    -> std::pair<entt::entity, const ComponentDef&>
{
  for (auto&& [entity, def] : registry.view<ComponentDef>().each()) {
    if (def.name == name) {
      return {entity, def};
    }
  }

  ThrowTraced(TactileError{"Invalid component name!"});
}

auto GetComponentDefName(const entt::registry& registry, const ComponentID id)
    -> const std::string&
{
  const auto& [entity, def] = GetComponentDef(registry, id);
  return def.name;
}

auto GetFirstAvailableComponentDef(const entt::registry& registry) -> Maybe<ComponentID>
{
  if (!registry.storage<ComponentDef>().empty()) {
    const auto entity = registry.view<ComponentDef>().front();
    const auto& def = registry.get<ComponentDef>(entity);
    return def.id;
  }
  else {
    return nothing;
  }
}

auto IsComponentValid(const entt::registry& registry, const ComponentID id) -> bool
{
  for (auto [entity, def] : registry.view<ComponentDef>().each()) {
    if (def.id == id) {
      return true;
    }
  }

  return false;
}

void CreateComponentAttribute(entt::registry& registry,
                              const ComponentID id,
                              const std::string& name)
{
  CreateComponentAttribute(registry, id, name, std::string{});
}

void CreateComponentAttribute(entt::registry& registry,
                              const ComponentID id,
                              const std::string& name,
                              const attribute_value& value)
{
  LogDebug("Adding attribute '{}' to component '{}'", name, id);

  auto [defEntity, def] = GetComponentDef(registry, id);
  def.attributes[name] = value;

  /* Updates existing components of the affected type to feature the new attribute */
  for (auto&& [entity, component] : registry.view<Component>().each()) {
    if (component.type == id) {
      component.values[name] = value;
    }
  }
}

void RemoveComponentAttribute(entt::registry& registry,
                              const ComponentID id,
                              const std::string_view name)
{
  LogDebug("Removing attribute '{}' from component '{}'", name, id);

  TACTILE_ASSERT(FindComponentDef(registry, id) != entt::null);
  TACTILE_ASSERT(IsComponentAttributeNameTaken(registry, id, name));

  const auto removeAttributeFrom = [name](auto& map) {
    std::erase_if(map, [name](const auto& pair) { return pair.first == name; });
  };

  auto [defEntity, def] = GetComponentDef(registry, id);
  removeAttributeFrom(def.attributes);

  for (auto&& [entity, component] : registry.view<Component>().each()) {
    if (component.type == id) {
      removeAttributeFrom(component.values);
    }
  }
}

void RenameComponentAttribute(entt::registry& registry,
                              const ComponentID id,
                              const std::string& current,
                              std::string updated)
{
  TACTILE_ASSERT(!IsComponentAttributeNameTaken(registry, id, updated));

  LogDebug("Renaming attribute '{}' in component '{}' to '{}'", current, id, updated);

  auto [defEntity, def] = GetComponentDef(registry, id);

  TACTILE_ASSERT(def.attributes.contains(current));
  auto value = def.attributes.at(current);

  def.attributes.erase(current);
  def.attributes[std::move(updated)] = std::move(value);
}

auto DuplicateComponentAttribute(entt::registry& registry,
                                 const ComponentID id,
                                 const std::string_view attribute) -> std::string
{
  LogDebug("Duplicating attribute '{}' in component '{}'", attribute, id);

  auto [defEntity, def] = GetComponentDef(registry, id);
  auto iter = GetComponentAttribute(registry, id, attribute);

  int suffix = 1;
  std::string candidateName;
  do {
    candidateName = fmt::format("{} ({})", attribute, suffix);
    ++suffix;
  } while (IsComponentAttributeNameTaken(registry, id, candidateName));

  def.attributes[candidateName] = iter->second;

  return candidateName;
}

void SetComponentAttributeType(entt::registry& registry,
                               const ComponentID id,
                               const std::string_view attribute,
                               const PropertyType type)
{
  LogVerbose("Setting type of attribute '{}' in component '{}' to '{}'",
             attribute,
             id,
             type);

  auto iter = GetComponentAttribute(registry, id, attribute);
  iter->second.reset_to_default(type);
}

void SetComponentAttributeValue(entt::registry& registry,
                                const ComponentID id,
                                const std::string_view attribute,
                                attribute_value value)
{
  auto iter = GetComponentAttribute(registry, id, attribute);

  TACTILE_ASSERT_MSG(iter->second.type() == value.type(),
                     "Requested default value had wrong type!");
  iter->second = std::move(value);
}

auto GetComponentAttributeType(const entt::registry& registry,
                               const ComponentID id,
                               const std::string_view attribute) -> PropertyType
{
  const auto iter = GetComponentAttribute(registry, id, attribute);
  return iter->second.type();
}

auto GetComponentAttributeValue(const entt::registry& registry,
                                const ComponentID id,
                                const std::string_view attribute) -> const attribute_value&
{
  const auto iter = GetComponentAttribute(registry, id, attribute);
  return iter->second;
}

auto IsComponentAttributeNameTaken(const entt::registry& registry,
                                   const ComponentID id,
                                   const std::string_view name) -> bool
{
  const auto& [entity, def] = GetComponentDef(registry, id);

  for (const auto& [attributeName, attribute] : def.attributes) {
    if (attributeName == name) {
      return true;
    }
  }

  return false;
}

auto GetComponentAttributeCount(const entt::registry& registry, const ComponentID id)
    -> usize
{
  const auto [entity, def] = GetComponentDef(registry, id);
  return def.attributes.size();
}

auto AddComponent(entt::registry& registry,
                  const ContextID context,
                  const ComponentID component) -> Component&
{
  auto& ctx = GetContext(registry, context);

  const auto componentEntity = registry.create();
  ctx.components.push_back(componentEntity);

  const auto [defEntity, def] = GetComponentDef(registry, component);
  auto& comp = registry.emplace<Component>(componentEntity);
  comp.type = component;
  comp.values = def.attributes;

  return comp;
}

auto RemoveComponent(entt::registry& registry,
                     const ContextID contextId,
                     const ComponentID componentId) -> RemoveComponentResult
{
  TACTILE_ASSERT(FindComponentDef(registry, componentId) != entt::null);
  TACTILE_ASSERT(HasComponent(registry, contextId, componentId));

  LogDebug("Removing component '{}' from context '{}'", componentId, contextId);

  auto& context = GetContext(registry, contextId);
  entt::entity match = entt::null;

  RemoveComponentResult snapshot;
  snapshot.context_id = contextId;
  snapshot.component_id = componentId;

  for (const auto componentEntity : context.components) {
    const auto& component = registry.get<Component>(componentEntity);
    if (component.type == componentId) {
      snapshot.values = component.values;
      match = componentEntity;

      registry.destroy(componentEntity);
    }
  }

  TACTILE_ASSERT(match != entt::null);
  std::erase_if(context.components, [match](const entt::entity componentEntity) {
    return componentEntity == match;
  });

  return snapshot;
}

void RestoreComponent(entt::registry& registry, RemoveComponentResult snapshot)
{
  TACTILE_ASSERT(FindComponentDef(registry, snapshot.component_id) != entt::null);
  TACTILE_ASSERT(!HasComponent(registry, snapshot.context_id, snapshot.component_id));

  LogDebug("Restoring component '{}' for context '{}'",
           snapshot.component_id,
           snapshot.context_id);

  auto& context = GetContext(registry, snapshot.context_id);

  const auto componentEntity = registry.create();
  context.components.push_back(componentEntity);

  auto& comp = registry.emplace<Component>(componentEntity);
  comp.type = snapshot.component_id;
  comp.values = std::move(snapshot.values);
}

void UpdateComponent(entt::registry& registry,
                     const ContextID contextId,
                     const ComponentID componentId,
                     const std::string_view attribute,
                     attribute_value value)
{
  auto& component = GetComponent(registry, contextId, componentId);

  auto iter = GetComponentValue(component, attribute);
  iter->second = std::move(value);
}

auto ResetComponent(entt::registry& registry,
                    const ContextID contextId,
                    const ComponentID componentId) -> ResetComponentResult
{
  TACTILE_ASSERT(HasComponent(registry, contextId, componentId));

  LogDebug("Resetting component '{}' in context '{}'", componentId, contextId);

  const auto& [defEntity, def] = GetComponentDef(registry, componentId);
  auto& component = GetComponent(registry, contextId, componentId);

  ResetComponentResult result;
  for (auto& [name, value] : component.values) {
    result.values[name] = value;
    value = def.attributes.at(name);
  }

  return result;
}

auto HasComponent(const entt::registry& registry,
                  const ContextID contextId,
                  const ComponentID componentId) -> bool
{
  const auto& context = GetContext(registry, contextId);

  for (const auto componentEntity : context.components) {
    const auto& component = registry.get<Component>(componentEntity);
    if (component.type == componentId) {
      return true;
    }
  }

  return false;
}

auto GetComponent(const entt::registry& registry,
                  const ContextID contextId,
                  const ComponentID componentId) -> const Component&
{
  const auto& context = GetContext(registry, contextId);

  for (const auto componentEntity : context.components) {
    const auto& component = registry.get<Component>(componentEntity);
    if (component.type == componentId) {
      return component;
    }
  }

  ThrowTraced(TactileError{"Context did not feature requested component!"});
}

auto GetAttribute(const Component& component, const std::string_view attribute)
    -> const attribute_value&
{
  if (const auto it = component.values.find(attribute); it != component.values.end()) {
    return it->second;
  }
  else {
    ThrowTraced(TactileError{"Did not find component attribute!"});
  }
}

auto GetComponentAttribute(const entt::registry& registry,
                           const ContextID contextId,
                           const ComponentID componentId,
                           const std::string_view attribute) -> const attribute_value&
{
  const auto& context = GetContext(registry, contextId);

  for (const auto componentEntity : context.components) {
    const auto& component = registry.get<Component>(componentEntity);
    if (component.type == componentId) {
      if (const auto it = component.values.find(attribute);
          it != component.values.end()) {
        return it->second;
      }
    }
  }

  ThrowTraced(TactileError{"Did not find component attribute!"});
}

auto GetComponentCount(const entt::registry& registry, const ContextID contextId) -> usize
{
  return GetContext(registry, contextId).components.size();
}

auto IsComponentNameTaken(const entt::registry& registry, const std::string_view name)
    -> bool
{
  for (auto&& [entity, component] : registry.view<ComponentDef>().each()) {
    if (component.name == name) {
      return true;
    }
  }

  return false;
}

}  // namespace Tactile::Sys
