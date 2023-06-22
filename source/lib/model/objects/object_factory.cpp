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

#include "object_factory.hpp"

#include "common/debug/assert.hpp"
#include "model/contexts/context_components.hpp"
#include "model/objects/object_components.hpp"
#include "model/systems/validation_system.hpp"

namespace tactile::sys {

auto create_object(Model& model, const ObjectType type) -> Entity
{
  const auto object_entity = model.create_entity();

  auto& object = model.add<Object>(object_entity);
  object.type = type;
  object.position = Float2 {0, 0};
  object.size = Float2 {0, 0};
  object.visible = true;

  auto& context = model.add<Context>(object_entity);
  context.name = "Object";

  TACTILE_ASSERT(is_object_entity(model, object_entity));
  return object_entity;
}

}  // namespace tactile::sys