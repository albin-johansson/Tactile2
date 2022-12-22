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

#include "add_property_dialog.hpp"

#include <entt/signal/dispatcher.hpp>
#include <imgui.h>

#include "core/context/context.hpp"
#include "core/context/context_info.hpp"
#include "core/context/context_manager.hpp"
#include "core/util/buffers.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/event/property_events.hpp"
#include "model/model.hpp"
#include "property_type_combo.hpp"

namespace tactile::ui {

AddPropertyDialog::AddPropertyDialog()
    : Dialog {"Add Property"}
{
}

void AddPropertyDialog::open(const UUID& contextId)
{
  mContextId = contextId;

  zero_buffer(mNameBuffer);
  mPropertyType = AttributeType::String;

  const auto& lang = get_current_language();
  set_title(lang.window.add_property);
  set_accept_button_label(lang.misc.add);

  make_visible();
}

void AddPropertyDialog::on_update(const DocumentModel&, entt::dispatcher&)
{
  const auto& lang = get_current_language();
  ImGui::InputTextWithHint("##Name",
                           lang.misc.property_name_hint.c_str(),
                           mNameBuffer.data(),
                           sizeof mNameBuffer);
  show_property_type_combo(mPropertyType);
}

void AddPropertyDialog::on_accept(entt::dispatcher& dispatcher)
{
  dispatcher.enqueue<AddPropertyEvent>(mContextId.value(),
                                       create_string_from_buffer(mNameBuffer),
                                       mPropertyType);
  mContextId.reset();
}

auto AddPropertyDialog::is_current_input_valid(const DocumentModel& model) const -> bool
{
  const auto& document = model.require_active_document();
  const auto& context = document.get_contexts().active_context();
  const auto name = create_string_view_from_buffer(mNameBuffer);
  return !name.empty() && !context.ctx().props().contains(name);
}

}  // namespace tactile::ui