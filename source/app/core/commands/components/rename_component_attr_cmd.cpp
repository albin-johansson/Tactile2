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

#include "rename_component_attr_cmd.hpp"

#include <utility>  // move

#include "core/systems/component_system.hpp"

namespace tactile {

RenameComponentAttrCmd::RenameComponentAttrCmd(RegistryRef registry,
                                               const ComponentID& id,
                                               std::string previousName,
                                               std::string updatedName)
    : ACommand{"Rename Component Attribute"}
    , mRegistry{registry}
    , mComponentId{id}
    , mPreviousName{std::move(previousName)}
    , mUpdatedName{std::move(updatedName)}
{}

void RenameComponentAttrCmd::undo()
{
  auto& registry = mRegistry.get();
  sys::rename_component_attribute(registry, mComponentId, mUpdatedName, mPreviousName);
}

void RenameComponentAttrCmd::redo()
{
  auto& registry = mRegistry.get();
  sys::rename_component_attribute(registry, mComponentId, mPreviousName, mUpdatedName);
}

}  // namespace tactile