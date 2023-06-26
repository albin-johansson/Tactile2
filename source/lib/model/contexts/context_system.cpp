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

#include "context_system.hpp"

#include "common/debug/assert.hpp"
#include "common/util/lookup.hpp"
#include "model/components/attached_component_system.hpp"
#include "model/components/component_components.hpp"
#include "model/entity_validation.hpp"

namespace tactile::sys {

auto copy_context(Model& model, const Context& src_context) -> Context
{
  Context new_context;
  new_context.name = src_context.name;
  new_context.props = src_context.props;

  new_context.comps.reserve(src_context.comps.size());
  for (const auto src_attached_component_entity: src_context.comps) {
    const auto& src_attached_component =
        model.get<AttachedComponent>(src_attached_component_entity);
    new_context.comps.push_back(copy_component(model, src_attached_component));
  }

  return new_context;
}

auto copy_component_values(const Model& model, const Entity definition_entity)
    -> HashMap<Entity, StringMap<Attribute>>
{
  TACTILE_ASSERT(is_component_entity(model, definition_entity));
  HashMap<Entity, StringMap<Attribute>> component_values;

  for (auto [context_entity, context]: model.each<Context>()) {
    for (const auto attached_component_entity: context.comps) {
      const auto& attached_component =
          model.get<AttachedComponent>(attached_component_entity);

      if (attached_component.definition == definition_entity) {
        component_values[context_entity] = attached_component.attributes;
      }
    }
  }

  return component_values;
}

auto copy_single_attribute_in_components(const Model& model,
                                         const Entity definition_entity,
                                         StringView attribute_name)
    -> HashMap<Entity, Attribute>
{
  TACTILE_ASSERT(is_component_entity(model, definition_entity));
  HashMap<Entity, Attribute> attributes;

  for (auto [attached_component_entity, attached_component]:
       model.each<AttachedComponent>()) {
    if (attached_component.definition == definition_entity) {
      attributes[attached_component_entity] =
          lookup_in(attached_component.attributes, attribute_name);
    }
  }

  return attributes;
}

}  // namespace tactile::sys
