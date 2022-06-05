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

#include "add_property_cmd.hpp"

#include <utility>  // move

#include "core/systems/context_system.hpp"
#include "core/systems/property_system.hpp"

namespace tactile {

AddPropertyCmd::AddPropertyCmd(RegistryRef registry,
                               std::string name,
                               const AttributeType type)
    : ACommand{"Add Property"}
    , mRegistry{registry}
    , mContextId{sys::current_context(mRegistry).id}
    , mName{std::move(name)}
    , mType{type}
{}

void AddPropertyCmd::undo()
{
  auto& context = sys::get_context(mRegistry, mContextId);
  sys::remove_property(mRegistry, context, mName);
}

void AddPropertyCmd::redo()
{
  auto& context = sys::get_context(mRegistry, mContextId);
  sys::add_property(mRegistry, context, mName, mType);
}

}  // namespace tactile
