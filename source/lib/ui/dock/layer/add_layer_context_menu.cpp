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

#include "add_layer_context_menu.hpp"

#include <imgui.h>

#include "model/events/layer_events.hpp"
#include "model/i18n/language_system.hpp"

namespace tactile::ui {

AddLayerContextMenu::AddLayerContextMenu()
    : ContextMenu {"AddLayerContextMenu"}
{
}

void AddLayerContextMenu::on_update(const Registry& registry, Dispatcher& dispatcher)
{
  const auto& strings = sys::get_current_language_strings(registry);

  if (ImGui::MenuItem(strings.action.tile_layer.c_str())) {
    dispatcher.enqueue<CreateLayerEvent>(LayerType::TileLayer);
  }

  if (ImGui::MenuItem(strings.action.object_layer.c_str())) {
    dispatcher.enqueue<CreateLayerEvent>(LayerType::ObjectLayer);
  }

  if (ImGui::MenuItem(strings.action.group_layer.c_str())) {
    dispatcher.enqueue<CreateLayerEvent>(LayerType::GroupLayer);
  }
}

}  // namespace tactile::ui