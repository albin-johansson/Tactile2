#include "object_system.hpp"

#include "core/components/object.hpp"

namespace tactile::sys {

auto find_object(const entt::registry& registry, const ObjectID id) -> entt::entity
{
  for (auto&& [entity, object] : registry.view<Object>().each()) {
    if (object.id == id) {
      return entity;
    }
  }

  return entt::null;
}

}  // namespace tactile::sys
