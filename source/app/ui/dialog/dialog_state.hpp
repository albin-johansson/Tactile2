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

#pragma once

#include "ui/dock/layer/add_layer_context_menu.hpp"
#include "ui/dock/property/dialogs/add_property_dialog.hpp"
#include "ui/dock/property/dialogs/change_property_type_dialog.hpp"
#include "ui/dock/property/dialogs/rename_property_dialog.hpp"

namespace tactile::ui {

struct [[deprecated]] DialogState final {
  AddPropertyDialog add_property;
  RenamePropertyDialog rename_property;
  ChangePropertyTypeDialog change_property_type;
};

}  // namespace tactile::ui
