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

#include "buttons.hpp"

#include <imgui.h>

#include "debug/assert.hpp"
#include "scoped.hpp"
#include "tooltips.hpp"
#include "ui/style/alignment.hpp"

namespace tactile::ui {

auto ui_button(const char* text,
               const char* tooltip,
               const bool enabled,
               const float width,
               const float height) -> bool
{
  TACTILE_ASSERT(text);

  const Disable disable {!enabled};

  const auto result = ImGui::Button(text, {width, height});

  if (tooltip) {
    ui_lazy_tooltip(text, tooltip);
  }

  if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  return result;
}

auto ui_icon_button(const char* icon, const char* tooltip, const bool enabled) -> bool
{
  const auto& style = ImGui::GetStyle();
  const float height = ImGui::GetFontSize() + style.ItemSpacing.y * 2.0f;
  const float width = height;
  return ui_button(icon, tooltip, enabled, width, height);
}

auto ui_centered_button(const char* text, const char* tooltip) -> bool
{
  TACTILE_ASSERT(text);

  const auto width = standard_component_width(text);
  center_next_item_horizontally(width);

  return ui_button(text, tooltip);
}

}  // namespace tactile::ui
