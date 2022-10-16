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

#include "model/cmd/property/rename_property.hpp"

#include <gtest/gtest.h>

#include "core/helpers/map_builder.hpp"
#include "misc/panic.hpp"

namespace tactile::test {

TEST(RenameProperty, Constructor)
{
  ASSERT_THROW(cmd::RenameProperty(nullptr, "", ""), TactileError);
}

TEST(RenameProperty, RedoUndo)
{
  auto document = MapBuilder::build().result();
  auto map = document->get_map_ptr();

  auto& props = map->ctx().props();
  props.add("foo", cen::colors::red);

  cmd::RenameProperty cmd {map, "foo", "bar"};
  cmd.redo();

  ASSERT_FALSE(props.contains("foo"));
  ASSERT_TRUE(props.contains("bar"));
  ASSERT_EQ(cen::colors::red, props.at("bar"));

  cmd.undo();

  ASSERT_TRUE(props.contains("foo"));
  ASSERT_FALSE(props.contains("bar"));
  ASSERT_EQ(cen::colors::red, props.at("foo"));
}

}  // namespace tactile::test
