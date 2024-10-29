// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstddef>    // size_t
#include <optional>   // optional, nullopt
#include <stdexcept>  // out_of_range
#include <utility>    // forward, move

#include <entt/entity/registry.hpp>

#include "tactile/base/prelude.hpp"
#include "tactile/core/debug/assert.hpp"
#include "tactile/core/entity/entity.hpp"

namespace tactile::core {

/**
 * Represents an ECS registry.
 */
class Registry final
{
 public:
  /**
   * Creates a new entity.
   *
   * \return
   * The assigned entity identifier.
   */
  [[nodiscard]] auto make_entity() -> EntityID;

  /**
   * Destroys an entity.
   *
   * \param entity_id The target entity.
   *
   * \return
   * True if the entity was valid; false otherwise.
   */
  auto destroy(EntityID entity_id) -> bool;

  /**
   * Adds a component to the registry itself.
   *
   * \tparam T    The component type.
   * \tparam Args The types of arguments that will be forwarded.
   *
   * \param args The arguments that will be forwarded to a constructor.
   *
   * \return
   * The attached component.
   */
  template <typename T, typename... Args>
  auto add(Args&&... args) -> T&
  {
    return mRegistry.ctx().insert_or_assign(T {std::forward<Args>(args)...});
  }

  /**
   * Adds a component to an entity.
   *
   * \tparam T    The component type.
   * \tparam Args The types of arguments that will be forwarded.
   *
   * \param entity The target entity.
   * \param args   The arguments that will be forwarded to a constructor.
   *
   * \return
   * The attached component.
   */
  template <typename T, typename... Args>
  auto add(const EntityID entity, Args&&... args) -> T&
  {
    return mRegistry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
  }

  /**
   * Removes a component from an entity (if it exists).
   *
   * \tparam T A component type.
   *
   * \param entity The target entity.
   */
  template <typename T>
  void erase(const EntityID entity)
  {
    if (has<T>(entity)) {
      mRegistry.erase<T>(entity);
    }
  }

  /**
   * Removes a component from an entity and returns it (if it exists).
   *
   * \note
   * Consider using the \c erase function instead if the returned component is
   * ignored, for improved efficiency.
   *
   * \tparam T A component type.
   *
   * \param entity The target entity.
   *
   * \return
   * The removed component; nothing if the entity didn't feature the component.
   */
  template <typename T>
  [[nodiscard]] auto detach(const EntityID entity) -> std::optional<T>
  {
    if (has<T>(entity)) {
      auto component = get<T>(entity);
      mRegistry.erase<T>(entity);
      return std::optional {std::move(component)};
    }

    return std::nullopt;
  }

  /**
   * Returns a component attached to registry, aka a "context" component.
   *
   * \tparam T A component type.
   *
   * \return
   * A component.
   */
  template <typename T>
  [[nodiscard]] auto get() -> T&
  {
    if (auto* component = find<T>()) {
      return *component;
    }

    throw std::out_of_range {"missing context component"};
  }

  /**
   * \copydoc get()
   */
  template <typename T>
  [[nodiscard]] auto get() const -> const T&
  {
    if (const auto* component = find<T>()) {
      return *component;
    }

    throw std::out_of_range {"missing context component"};
  }

  /**
   * Returns a component attached to an entity.
   *
   * \tparam T A component type.
   *
   * \param entity The target entity.
   *
   * \return
   * A component.
   */
  template <typename T>
  [[nodiscard]] auto get(const EntityID entity) -> T&
  {
    if (auto* component = find<T>(entity)) [[likely]] {
      return *component;
    }

    throw std::out_of_range {"bad entity"};
  }

  /**
   * \copydoc get(EntityID)
   */
  template <typename T>
  [[nodiscard]] auto get(const EntityID entity) const -> const T&
  {
    if (const auto* component = find<T>(entity)) [[likely]] {
      return *component;
    }

    throw std::out_of_range {"bad entity"};
  }

  /**
   * Attempts to return a given context component.
   *
   * \tparam T The context component type.
   *
   * \return
   * A pointer to the context component if found; a null pointer otherwise.
   */
  template <typename T>
  [[nodiscard]] auto find() -> T*
  {
    return mRegistry.ctx().find<T>();
  }

  /**
   * \copydoc find()
   */
  template <typename T>
  [[nodiscard]] auto find() const -> const T*
  {
    return mRegistry.ctx().find<T>();
  }

  /**
   * Attempts to return a component attached to an entity.
   *
   * \tparam T A component type.
   *
   * \param entity The target entity.
   *
   * \return
   * A component if found; a null pointer otherwise.
   */
  template <typename T>
  [[nodiscard]] auto find(const EntityID entity) -> T*
  {
    return mRegistry.try_get<T>(entity);
  }

  /**
   * \copydoc find(EntityID)
   */
  template <typename T>
  [[nodiscard]] auto find(const EntityID entity) const -> const T*
  {
    return mRegistry.try_get<T>(entity);
  }

  /**
   * Indicates whether the registry features a given context component.
   *
   * \tparam T The component type to look for.
   *
   * \return
   * True if the registry has the specified context component; false otherwise.
   */
  template <typename T>
  [[nodiscard]] auto has() const -> bool
  {
    return find<T>() != nullptr;
  }

  /**
   * Indicates whether an entity features a given component.
   *
   * \tparam T The type of the component to look for.
   *
   * \param entity The entity to check.
   *
   * \return
   * True if the entity has the component; false otherwise.
   */
  template <typename T>
  [[nodiscard]] auto has(const EntityID entity) const -> bool
  {
    return mRegistry.all_of<T>(entity);
  }

  /**
   * Returns an iterable object for each entity with the specified components.
   *
   * \tparam Ts The component types.
   *
   * \return
   * An iterable object.
   */
  template <typename... Ts>
  [[nodiscard]] auto each()
  {
    return mRegistry.view<Ts...>().each();
  }

  /**
   * \copydoc each
   */
  template <typename... Ts>
  [[nodiscard]] auto each() const
  {
    return mRegistry.view<Ts...>().each();
  }

  /**
   * Returns the number of instances of a given component.
   *
   * \tparam T A component type.
   *
   * \return
   * The number of component instances.
   */
  template <typename T>
  [[nodiscard]] auto count() const -> std::size_t
  {
    if (const auto* storage = mRegistry.storage<T>()) {
      return storage->size();
    }

    return 0;
  }

  /**
   * Returns the total number of components in the registry.
   *
   * \note
   * This function is mainly intended for debugging/testing purposes.
   *
   * \return
   * The number of components.
   */
  [[nodiscard]] auto count() const -> std::size_t;

  /**
   * Indicates whether an entity identifier is valid.
   *
   * \param entity The entity to check.
   *
   * \return
   * True if the entity is valid; false otherwise.
   */
  [[nodiscard]] auto is_valid(EntityID entity) const -> bool;

 private:
  entt::registry mRegistry {};
};

}  // namespace tactile::core
