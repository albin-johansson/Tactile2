// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/meta/update_property_command.hpp"

#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/meta/meta.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

// tactile::core::UpdatePropertyCommand::redo
// tactile::core::UpdatePropertyCommand::undo
TEST(UpdatePropertyCommand, RedoUndo)
{
  const std::string name {"abc"};
  const Attribute old_value {42};
  const Attribute new_value {43};

  auto document = MapDocument::make(kOrthogonalMapSpec).value();

  auto& registry = document.get_registry();
  const auto map_entity = registry.get<CDocumentInfo>().root;

  auto& meta = registry.get<CMeta>(map_entity);
  meta.properties[name] = old_value;

  ASSERT_TRUE(meta.properties.contains(name));
  ASSERT_EQ(meta.properties.at(name), old_value);
  ASSERT_EQ(meta.properties.size(), 1);

  UpdatePropertyCommand command {&document, map_entity, name, new_value};
  command.redo();

  EXPECT_TRUE(meta.properties.contains(name));
  EXPECT_EQ(meta.properties.at(name), new_value);
  EXPECT_EQ(meta.properties.size(), 1);

  command.undo();

  EXPECT_TRUE(meta.properties.contains(name));
  EXPECT_EQ(meta.properties.at(name), old_value);
  EXPECT_EQ(meta.properties.size(), 1);
}

// tactile::core::UpdatePropertyCommand::merge_with
TEST(UpdatePropertyCommand, MergeWith)
{
  const std::string name {"abc"};

  const Attribute value0 {"0"};
  const Attribute value1 {"1"};
  const Attribute value2 {"2"};
  const Attribute value3 {"3"};

  auto document = MapDocument::make(kOrthogonalMapSpec).value();

  auto& registry = document.get_registry();
  const auto map_entity = registry.get<CDocumentInfo>().root;

  auto& meta = registry.get<CMeta>(map_entity);
  meta.properties[name] = value0;

  ASSERT_TRUE(meta.properties.contains(name));
  ASSERT_EQ(meta.properties.at(name), value0);
  ASSERT_EQ(meta.properties.size(), 1);

  UpdatePropertyCommand set_value1 {&document, map_entity, name, value1};
  UpdatePropertyCommand set_value2 {&document, map_entity, name, value2};

  set_value1.redo();

  EXPECT_TRUE(meta.properties.contains(name));
  EXPECT_EQ(meta.properties.at(name), value1);
  EXPECT_EQ(meta.properties.size(), 1);

  set_value2.redo();
  ASSERT_TRUE(set_value1.merge_with(&set_value2));

  EXPECT_TRUE(meta.properties.contains(name));
  EXPECT_EQ(meta.properties.at(name), value2);
  EXPECT_EQ(meta.properties.size(), 1);

  set_value1.undo();

  EXPECT_TRUE(meta.properties.contains(name));
  EXPECT_EQ(meta.properties.at(name), value0);
  EXPECT_EQ(meta.properties.size(), 1);

  set_value1.redo();

  EXPECT_TRUE(meta.properties.contains(name));
  EXPECT_EQ(meta.properties.at(name), value2);
  EXPECT_EQ(meta.properties.size(), 1);
}

}  // namespace
}  // namespace tactile::core
