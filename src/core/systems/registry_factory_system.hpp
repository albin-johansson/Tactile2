#pragma once

#include <entt.hpp>  // registry

namespace Tactile::Sys {

/**
 * \brief Creates a registry with the expected context variables.
 *
 * \details The created registry will feature the following context variables:
 * - `Map`
 * - `ActiveLayer`
 * - `ActiveTileset`
 * - `ActivePropertyContext`
 * - `Viewport`
 * - `PropertyContext`
 *
 * \return a registry.
 *
 * \since 0.2.0
 */
[[nodiscard]] auto MakeRegistry() -> entt::registry;

}  // namespace Tactile::Sys
