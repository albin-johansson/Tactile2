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

#include "create_component_def_cmd.hpp"

#include <utility>  // move

#include "core/systems/component_system.hpp"

namespace tactile {

CreateComponentDefCmd::CreateComponentDefCmd(RegistryRef registry, std::string name)
    : ACommand{"Create Component Definition"}
    , mRegistry{registry}
    , mName{std::move(name)}
{}

void CreateComponentDefCmd::undo()
{
  auto& registry = mRegistry.get();
  sys::remove_component_def(registry, mComponentId.value());
}

void CreateComponentDefCmd::redo()
{
  auto& registry = mRegistry.get();
  if (!mComponentId) {
    mComponentId = sys::make_component_def(registry, mName);
  }
  else {
    sys::make_component_def(mRegistry, *mComponentId, mName);
  }
}

}  // namespace tactile