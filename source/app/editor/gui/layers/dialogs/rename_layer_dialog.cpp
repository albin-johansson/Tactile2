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

#include "rename_layer_dialog.hpp"

#include <utility>  // move

#include "editor/events/layer_events.hpp"

namespace tactile {

RenameLayerDialog::RenameLayerDialog() : AStringInputDialog{"Rename Layer"}
{
  set_accept_button_label("Rename");
}

void RenameLayerDialog::show(const LayerID id, std::string oldName)
{
  mTargetId = id;
  mOldName = std::move(oldName);
  AStringInputDialog::show(*mOldName);
}

void RenameLayerDialog::on_accept(entt::dispatcher& dispatcher)
{
  dispatcher.enqueue<RenameLayerEvent>(mTargetId.value(), std::string{current_input()});
}

auto RenameLayerDialog::validate(const DocumentModel&, std::string_view input) const
    -> bool
{
  return !input.empty() && mOldName != input;
}

}  // namespace tactile
