// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/meta/rename_property_command.hpp"

#include <gtest/gtest.h>

#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/meta/meta.hpp"
#include "test/document_testing.hpp"

namespace tactile::test {

// tactile::RenamePropertyCommand::redo
// tactile::RenamePropertyCommand::undo
TEST(RenamePropertyCommand, RedoUndo)
{
  const String old_name {"foo"};
  const String new_name {"bar"};

  MapDocument document {kOrthogonalMapSpec};
  const auto map_entity = document.get_root_entity();

  auto& registry = document.get_registry();
  auto& meta = registry.get<CMeta>(map_entity);
  meta.properties.insert_or_assign(old_name, Attribute {42});

  ASSERT_TRUE(meta.properties.contains(old_name));
  ASSERT_FALSE(meta.properties.contains(new_name));

  RenamePropertyCommand command {&document, map_entity, old_name, new_name};
  command.redo();

  EXPECT_FALSE(meta.properties.contains(old_name));
  EXPECT_TRUE(meta.properties.contains(new_name));

  command.undo();

  EXPECT_TRUE(meta.properties.contains(old_name));
  EXPECT_FALSE(meta.properties.contains(new_name));
}

// tactile::RenamePropertyCommand::merge_with
TEST(RenamePropertyCommand, MergeWith)
{
  const String name1 {"A"};
  const String name2 {"B"};
  const String name3 {"C"};

  MapDocument document {kOrthogonalMapSpec};
  const auto map_entity = document.get_root_entity();

  auto& registry = document.get_registry();
  auto& meta = registry.get<CMeta>(map_entity);
  meta.properties.insert_or_assign(name1, Attribute {"attr"});

  ASSERT_TRUE(meta.properties.contains(name1));
  ASSERT_FALSE(meta.properties.contains(name2));
  ASSERT_FALSE(meta.properties.contains(name3));

  RenamePropertyCommand cmd1 {&document, map_entity, name1, name2};
  RenamePropertyCommand cmd2 {&document, map_entity, name2, name3};

  cmd1.redo();
  EXPECT_FALSE(meta.properties.contains(name1));
  EXPECT_TRUE(meta.properties.contains(name2));
  EXPECT_FALSE(meta.properties.contains(name3));

  cmd2.redo();
  EXPECT_TRUE(cmd1.merge_with(&cmd2));

  EXPECT_FALSE(meta.properties.contains(name1));
  EXPECT_FALSE(meta.properties.contains(name2));
  EXPECT_TRUE(meta.properties.contains(name3));

  cmd1.undo();
  EXPECT_TRUE(meta.properties.contains(name1));
  EXPECT_FALSE(meta.properties.contains(name2));
  EXPECT_FALSE(meta.properties.contains(name3));

  cmd1.redo();
  EXPECT_FALSE(meta.properties.contains(name1));
  EXPECT_FALSE(meta.properties.contains(name2));
  EXPECT_TRUE(meta.properties.contains(name3));
}

}  // namespace tactile::test