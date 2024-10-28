// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/meta/remove_property_command.hpp"

#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/meta/meta.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

// tactile::core::RemovePropertyCommand::redo
// tactile::core::RemovePropertyCommand::undo
TEST(RemovePropertyCommand, RedoUndo)
{
  const std::string name {"xyz"};
  const Attribute value {123};

  auto document = MapDocument::make(kOrthogonalMapSpec).value();

  auto& registry = document.get_registry();
  const auto map_entity = registry.get<CDocumentInfo>().root;

  auto& meta = registry.get<CMeta>(map_entity);
  meta.properties.insert_or_assign(name, value);

  ASSERT_TRUE(meta.properties.contains(name));
  ASSERT_EQ(meta.properties.at(name), value);
  ASSERT_EQ(meta.properties.size(), 1);

  RemovePropertyCommand command {&document, map_entity, name};

  command.redo();
  EXPECT_FALSE(meta.properties.contains(name));
  EXPECT_EQ(meta.properties.size(), 0);

  command.undo();
  EXPECT_TRUE(meta.properties.contains(name));
  EXPECT_EQ(meta.properties.at(name), value);
  EXPECT_EQ(meta.properties.size(), 1);
}

}  // namespace
}  // namespace tactile::core
