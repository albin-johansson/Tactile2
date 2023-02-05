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

#include <entt/entity/entity.hpp>

#include "common/type/string.hpp"
#include "common/type/uuid.hpp"
#include "core/attribute.hpp"

namespace tactile {

struct ShowNewPropertyDialogEvent final {};

struct ShowChangePropertyTypeDialogEvent final {
  String name;
  AttributeType current_type {};
};

struct ShowRenamePropertyDialogEvent final {
  String current_name;
};

struct AddPropertyEvent final {
  UUID context_id {};     /// Target context.
  String name;            /// The property name.
  AttributeType type {};  /// The property type.
};

struct RemovePropertyEvent final {
  UUID context_id {};  /// Target context.
  String name;         ///< Name of the property in the current context.
};

struct RenamePropertyEvent final {
  UUID context_id {};  /// Target context.
  String old_name;     ///< Current property name.
  String new_name;     ///< Requested new property name.
};

struct UpdatePropertyEvent final {
  UUID context_id {};  /// Target context.
  String name;         ///< Name of property to modify.
  Attribute value;     ///< Updated value of the property.
};

struct ChangePropertyTypeEvent final {
  UUID context_id {};     /// Target context.
  String name;            ///< Name of property to modify.
  AttributeType type {};  ///< Requested new property type.
};

struct InspectContextEvent final {
  UUID context_id {};  /// Target context.
};

}  // namespace tactile
