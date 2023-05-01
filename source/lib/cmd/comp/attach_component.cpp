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

#include "attach_component.hpp"

#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/model.hpp"
#include "model/systems/component_system.hpp"

namespace tactile::cmd {

AttachComponent::AttachComponent(const Entity context_entity,
                                 const Entity definition_entity)
    : mContextEntity {context_entity},
      mDefinitionEntity {definition_entity}
{
}

void AttachComponent::undo()
{
  auto& model = get_model();
  sys::detach_component(model, mContextEntity, mDefinitionEntity);
}

void AttachComponent::redo()
{
  auto& model = get_model();
  sys::attach_component(model, mContextEntity, mDefinitionEntity);
}

auto AttachComponent::get_name() const -> String
{
  const auto& lang = get_current_language();
  return lang.cmd.attach_comp;
}

}  // namespace tactile::cmd