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

#include "rename_property_dialog.hpp"

#include <utility>  // move

#include <entt/signal/dispatcher.hpp>

#include "core/ctx/context.hpp"
#include "core/ctx/context_manager.hpp"
#include "core/ctx/property_bundle.hpp"
#include "core/event/property_events.hpp"
#include "core/model.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"

namespace tactile::ui {

RenamePropertyDialog::RenamePropertyDialog()
    : AStringInputDialog {"Rename Property"}
{}

void RenamePropertyDialog::open(const UUID& context_id, std::string previous_name)
{
  mContextId = context_id;

  const auto& lang = get_current_language();
  set_title(lang.window.rename_property);
  set_accept_button_label(lang.misc.rename);

  show(std::move(previous_name));
}

void RenamePropertyDialog::on_accept(entt::dispatcher& dispatcher)
{
  dispatcher.enqueue<RenamePropertyEvent>(mContextId.value(),
                                          previous_input(),
                                          std::string {current_input()});
  mContextId.reset();
}

auto RenamePropertyDialog::validate(const DocumentModel& model,
                                    std::string_view input) const -> bool
{
  const auto& document = model.require_active_document();
  const auto& context = document.get_contexts().active_context();
  return !input.empty() && !context.get_props().contains(input);
}

}  // namespace tactile::ui
