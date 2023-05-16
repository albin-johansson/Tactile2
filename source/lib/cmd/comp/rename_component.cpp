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

#include "rename_component.hpp"

#include <utility>  // move

#include "core/component.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/context.hpp"

namespace tactile::cmd {

RenameComponent::RenameComponent(const Entity definition_entity, String new_name)
    : mDefinitionEntity {definition_entity},
      mNewName {std::move(new_name)}
{
}

void RenameComponent::undo()
{
  auto& model = get_global_model();

  auto& definition = model.get<ComponentDefinition>(mDefinitionEntity);
  definition.name = mOldName.value();

  mOldName.reset();
}

void RenameComponent::redo()
{
  auto& model = get_global_model();

  auto& definition = model.get<ComponentDefinition>(mDefinitionEntity);
  mOldName = definition.name;

  definition.name = mNewName;
}

auto RenameComponent::get_name() const -> String
{
  const auto& lang = get_current_language();
  return lang.cmd.rename_comp;
}

}  // namespace tactile::cmd