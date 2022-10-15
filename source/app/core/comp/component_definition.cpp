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

#include "component_definition.hpp"

#include <utility>  // move

#include "core/util/assoc.hpp"
#include "core/util/fmt.hpp"
#include "core/util/string.hpp"
#include "misc/panic.hpp"

namespace tactile {

ComponentDefinition::ComponentDefinition() = default;

ComponentDefinition::ComponentDefinition(const UUID& id)
    : mId {id}
{
}

auto ComponentDefinition::instantiate() const -> Component
{
  return {mId, mAttributes};
}

void ComponentDefinition::add(String key, AttributeType type)
{
  if (!has_key(mAttributes, key)) {
    mAttributes[std::move(key)].reset_to_default(type);
  }
  else {
    throw TactileError {"Attribute key must be unique!"};
  }
}

void ComponentDefinition::add(String key, Attribute value)
{
  if (!has_key(mAttributes, key)) {
    mAttributes[std::move(key)] = std::move(value);
  }
  else {
    throw TactileError {"Attribute key must be unique!"};
  }
}

void ComponentDefinition::update(StringView key, Attribute value)
{
  auto& attribute = lookup_in(mAttributes, key);
  attribute = std::move(value);
}

auto ComponentDefinition::remove(StringView key) -> bool
{
  if (const auto iter = find_in(mAttributes, key); iter != mAttributes.end()) {
    mAttributes.erase(iter);
    return true;
  }
  else {
    return false;
  }
}

auto ComponentDefinition::rename(StringView current, String updated) -> bool
{
  if (has_key(mAttributes, updated)) {
    throw TactileError {"Attribute name must be unique!"};
  }

  if (const auto iter = find_in(mAttributes, current); iter != mAttributes.end()) {
    auto value = iter->second;
    mAttributes.erase(iter);
    mAttributes[std::move(updated)] = std::move(value);
    return true;
  }
  else {
    return false;
  }
}

auto ComponentDefinition::duplicate(StringView key) -> String
{
  auto value = lookup_in(mAttributes, key);

  int suffix = 1;
  String new_key;
  do {
    new_key = format_str("{} ({})", key, suffix);
    ++suffix;
  } while (has_key(mAttributes, new_key));

  mAttributes[new_key] = std::move(value);
  return new_key;
}

auto ComponentDefinition::at(StringView key) const -> const Attribute&
{
  return lookup_in(mAttributes, key);
}

auto ComponentDefinition::has(StringView key) const -> bool
{
  return has_key(mAttributes, key);
}

void ComponentDefinition::set_name(String name)
{
  mName = std::move(name);
}

auto ComponentDefinition::name() const -> const String&
{
  return mName;
}

auto ComponentDefinition::size() const -> usize
{
  return mAttributes.size();
}

auto ComponentDefinition::empty() const -> bool
{
  return mAttributes.empty();
}

auto ComponentDefinition::uuid() const -> const UUID&
{
  return mId;
}

}  // namespace tactile
