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

#include "set_object_tag_cmd.hpp"

#include <utility>  // move

#include "editor/lang/language.hpp"
#include "editor/lang/strings.hpp"
#include "misc/panic.hpp"

namespace tactile {

SetObjectTagCmd::SetObjectTagCmd(Shared<Object> object, std::string tag)
    : mObject {std::move(object)}
    , mNewTag {std::move(tag)}
{
  if (!mObject) {
    throw TactileError {"Invalid null object!"};
  }
}

void SetObjectTagCmd::undo()
{
  mObject->set_tag(mOldTag.value());
  mOldTag.reset();
}

void SetObjectTagCmd::redo()
{
  mOldTag = mObject->get_tag();
  mObject->set_tag(mNewTag);
}

auto SetObjectTagCmd::merge_with(const ICommand* cmd) -> bool
{
  if (const auto* other = dynamic_cast<const SetObjectTagCmd*>(cmd)) {
    if (mObject->get_uuid() == other->mObject->get_uuid()) {
      mNewTag = other->mNewTag;
      return true;
    }
  }

  return false;
}

auto SetObjectTagCmd::get_name() const -> std::string
{
  const auto& lang = get_current_language();
  return lang.cmd.update_object_tag;
}

}  // namespace tactile