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

#pragma once

#include <imgui.h>

#include "common/type/dispatcher.hpp"
#include "model/model.hpp"

namespace tactile::ui {

/// Updates the root dock space, call this function once each frame.
void update_dock_space(const Model& model);

/// Loads the default layout for a dock space.
void load_default_layout(const Model& model, ImGuiID id);

/// Resets all dock space widgets to the default layout.
void reset_layout(const Model& model, Dispatcher& dispatcher);

/// Resets the layout and regenerates the 'imgui.ini' file if it is missing.
void check_for_missing_layout_file(const Model& model, Dispatcher& dispatcher);

}  // namespace tactile::ui
