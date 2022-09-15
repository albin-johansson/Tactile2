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

#include "add_property.hpp"

#include <utility>  // move

#include "core/ctx/context.hpp"
#include "core/ctx/context_info.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "misc/panic.hpp"

namespace tactile::cmd {

AddProperty::AddProperty(Shared<IContext> context,
                         std::string name,
                         const AttributeType type)
    : mContext {std::move(context)}
    , mName {std::move(name)}
    , mType {type}
{
  if (!mContext) {
    throw TactileError {"Invalid null context!"};
  }
}

void AddProperty::undo()
{
  auto& props = mContext->ctx().props();
  props.remove(mName);
}

void AddProperty::redo()
{
  auto& props = mContext->ctx().props();
  props.add(mName, mType);
}

auto AddProperty::get_name() const -> std::string
{
  const auto& lang = get_current_language();
  return lang.cmd.add_property;
}

}  // namespace tactile::cmd
