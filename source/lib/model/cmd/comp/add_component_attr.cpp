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

#include "add_component_attr.hpp"

#include <utility>  // move

#include "core/component/component_index.hpp"
#include "core/context/context_manager.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/document/document.hpp"
#include "tactile/core/debug/error.hpp"

namespace tactile::cmd {

AddComponentAttr::AddComponentAttr(Document* document,
                                   const UUID& component_id,
                                   String name)
    : mDocument {document},
      mComponentId {component_id},
      mName {std::move(name)}
{
  if (!mDocument) {
    throw Error {"Invalid null document!"};
  }
}

void AddComponentAttr::undo()
{
  auto component_index = mDocument->get_component_index_ptr();
  auto& component_def = component_index->get_comp(mComponentId);
  component_def.remove_attr(mName);

  auto& context_manager = mDocument->get_contexts();
  context_manager.on_removed_component_attr(component_def.get_uuid(), mName);
}

void AddComponentAttr::redo()
{
  auto component_index = mDocument->get_component_index_ptr();
  auto& component_def = component_index->get_comp(mComponentId);

  component_def.add_attr(mName);
  const auto& value = component_def.get_attr(mName);

  auto& context_manager = mDocument->get_contexts();
  context_manager.on_new_component_attr(component_def.get_uuid(), mName, value);
}

auto AddComponentAttr::get_name() const -> String
{
  const auto& lang = get_current_language();
  return lang.cmd.create_comp_attr;
}

}  // namespace tactile::cmd