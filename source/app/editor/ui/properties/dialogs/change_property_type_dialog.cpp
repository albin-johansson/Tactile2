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

#include "change_property_type_dialog.hpp"

#include <utility>  // move

#include <entt/signal/dispatcher.hpp>
#include <imgui.h>

#include "core/event/property_events.hpp"
#include "editor/lang/language.hpp"
#include "editor/lang/strings.hpp"
#include "editor/ui/properties/dialogs/property_type_combo.hpp"

namespace tactile::ui {

ChangePropertyTypeDialog::ChangePropertyTypeDialog()
    : ADialog {"Change Property Type"}
{}

void ChangePropertyTypeDialog::show(const UUID&         context_id,
                                    std::string         name,
                                    const AttributeType type)
{
  mContextId = context_id;
  mPropertyName = std::move(name);
  mCurrentType = type;
  mPreviousType = type;

  const auto& lang = get_current_language();
  set_title(lang.window.change_property_type);
  set_accept_button_label(lang.misc.change);

  make_visible();
}

void ChangePropertyTypeDialog::on_update(const DocumentModel&, entt::dispatcher&)
{
  const auto& lang = get_current_language();

  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted(lang.misc.type.c_str());

  ImGui::SameLine();
  show_property_type_combo(mPreviousType.value(), mCurrentType);
}

void ChangePropertyTypeDialog::on_accept(entt::dispatcher& dispatcher)
{
  dispatcher.enqueue<ChangePropertyTypeEvent>(mContextId.value(),
                                              mPropertyName.value(),
                                              mCurrentType);
  mContextId.reset();
}

auto ChangePropertyTypeDialog::is_current_input_valid(const DocumentModel&) const -> bool
{
  return mCurrentType != mPreviousType.value();
}

}  // namespace tactile::ui
