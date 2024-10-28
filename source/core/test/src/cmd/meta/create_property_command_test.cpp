// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/meta/create_property_command.hpp"

#include <optional>  // optional

#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/meta/meta.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

// tactile::core::CreatePropertyCommand::redo
// tactile::core::CreatePropertyCommand::undo
TEST(CreatePropertyCommand, RedoUndo)
{
  auto document = MapDocument::make(kOrthogonalMapSpec).value();

  const auto& registry = document.get_registry();
  const auto map_entity = registry.get<CDocumentInfo>().root;

  const auto& meta = registry.get<CMeta>(map_entity);
  ASSERT_EQ(meta.properties.size(), 0);
  ASSERT_EQ(meta.components.size(), 0);

  const std::string property_name {"foo"};
  const Attribute property_value {42};

  CreatePropertyCommand command {&document, map_entity, property_name, property_value};

  EXPECT_FALSE(meta.properties.contains(property_name));
  EXPECT_EQ(meta.properties.size(), 0);
  EXPECT_EQ(meta.components.size(), 0);

  command.redo();

  EXPECT_TRUE(meta.properties.contains(property_name));
  EXPECT_EQ(meta.properties.at(property_name), property_value);
  EXPECT_EQ(meta.properties.size(), 1);
  EXPECT_EQ(meta.components.size(), 0);

  command.undo();

  EXPECT_FALSE(meta.properties.contains(property_name));
  EXPECT_EQ(meta.properties.size(), 0);
  EXPECT_EQ(meta.components.size(), 0);
}

}  // namespace
}  // namespace tactile::core
