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

#include "move_object_cmd.hpp"

#include "core/components/object.hpp"

namespace tactile {

move_object_cmd::move_object_cmd(registry_ref registry,
                                 const object_id id,
                                 const float oldX,
                                 const float oldY,
                                 const float newX,
                                 const float newY)
    : object_cmd{"Move Object", registry, id}
    , mOldX{oldX}
    , mOldY{oldY}
    , mNewX{newX}
    , mNewY{newY}
{}

void move_object_cmd::undo()
{
  auto& object = target_object();
  object.x = mOldX;
  object.y = mOldY;
}

void move_object_cmd::redo()
{
  auto& object = target_object();
  object.x = mNewX;
  object.y = mNewY;
}

}  // namespace tactile
