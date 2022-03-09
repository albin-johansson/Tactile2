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

#include "add_layer_context_menu.hpp"

#include <imgui.h>

#include "editor/events/layer_events.hpp"
#include "editor/gui/icons.hpp"

namespace tactile {

add_layer_context_menu::add_layer_context_menu() : ContextMenu{"AddLayerContextMenu"} {}

void add_layer_context_menu::on_update(const DocumentModel&, entt::dispatcher& dispatcher)
{
  if (ImGui::Selectable(TAC_ICON_TILE_LAYER " Tile layer")) {
    dispatcher.enqueue<AddLayerEvent>(LayerType::tile_layer);
  }

  if (ImGui::Selectable(TAC_ICON_OBJECT_LAYER " Object layer")) {
    dispatcher.enqueue<AddLayerEvent>(LayerType::object_layer);
  }

  if (ImGui::Selectable(TAC_ICON_GROUP_LAYER " Group layer")) {
    dispatcher.enqueue<AddLayerEvent>(LayerType::group_layer);
  }
}

}  // namespace tactile