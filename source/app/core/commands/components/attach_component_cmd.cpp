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

#include "attach_component_cmd.hpp"

#include "core/documents/document.hpp"
#include "core/systems/component_system.hpp"
#include "misc/panic.hpp"

namespace tactile {

AttachComponentCmd::AttachComponentCmd(ADocument* document,
                                       const UUID& contextId,
                                       const UUID& componentId)
    : ACommand{"Attach Component"}
    , mDocument{document}
    , mContextId{contextId}
    , mComponentId{componentId}
{
  if (!mDocument) {
    throw TactileError{"Invalid null document!"};
  }
}

void AttachComponentCmd::undo()
{
  auto& registry = mDocument->get_registry();
  sys::remove_component(registry, mContextId, mComponentId);
}

void AttachComponentCmd::redo()
{
  auto& registry = mDocument->get_registry();
  sys::attach_component(registry, mContextId, mComponentId);
}

}  // namespace tactile