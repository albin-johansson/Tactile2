/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2023 Albin Johansson
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

#pragma once

#include "core/containers/hash_map.hpp"
#include "tactile/core/containers/string.hpp"
#include "tactile/core/containers/string_map.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/registry.hpp"
#include "model/contexts/context_components.hpp"

namespace tactile::sys {

/// Creates a copy of the context component attached to a context entity.
[[nodiscard]] auto copy_context(Registry& registry, const Context& src_context)
    -> Context;

/**
 * Copies the attributes of all components of a specific component type.
 *
 * \param model             the associated registry.
 * \param definition_entity a component definition entity.
 *
 * \return a map that associates context entities to attribute values.
 */
[[nodiscard]] auto copy_component_values(const Registry& registry,
                                         Entity definition_entity)
    -> HashMap<Entity, StringMap<Attribute>>;

/**
 * Copies a specific attribute in all components of a specific component type.
 *
 * \param model             the associated registry.
 * \param definition_entity a component definition entity.
 * \param attribute_name    the name of the attribute to copy.
 *
 * \return a map that associates component entities to attributes.
 */
[[nodiscard]] auto copy_single_attribute_in_components(const Registry& registry,
                                                       Entity definition_entity,
                                                       StringView attribute_name)
    -> HashMap<Entity, Attribute>;

}  // namespace tactile::sys
