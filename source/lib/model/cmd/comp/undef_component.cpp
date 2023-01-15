/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
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

#include "undef_component.hpp"

#include <utility>  // move

#include "common/debug/panic.hpp"
#include "core/component/component_index.hpp"
#include "core/context/context_info.hpp"
#include "core/context/context_manager.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/document/document.hpp"

namespace tactile::cmd {

UndefComponent::UndefComponent(Document* document, const UUID& component_id)
    : mDocument {document},
      mComponentId {component_id}
{
  if (!mDocument) {
    throw TactileError {"Invalid null document!"};
  }
}

void UndefComponent::undo()
{
  auto component_index = mDocument->get_component_index_ptr();

  component_index->restore(std::move(mPreviousDef.value()));
  mPreviousDef.reset();

  // Restores previously removed components
  auto& contexts = mDocument->get_contexts();
  for (auto [context_id, component]: mRemovedComponents) {
    auto& context = contexts.at(context_id);
    context.get_ctx().attach_component(std::move(component));
  }

  mRemovedComponents.clear();
}

void UndefComponent::redo()
{
  auto component_index = mDocument->get_component_index_ptr();

  mPreviousDef = component_index->at(mComponentId);
  component_index->remove(mComponentId);

  auto& contexts = mDocument->get_contexts();
  mRemovedComponents = contexts.on_undef_comp(mComponentId);
}

auto UndefComponent::get_name() const -> String
{
  const auto& lang = get_current_language();
  return lang.cmd.undef_comp;
}

}  // namespace tactile::cmd