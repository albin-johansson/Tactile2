// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/duplicate_layer_command.hpp"

#include <optional>  // optional
#include <utility>   // move

#include <gtest/gtest.h>

#include "tactile/core/cmd/layer/create_layer_command.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer.hpp"
#include "tactile/core/map/map.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class DuplicateLayerCommandTest : public testing::Test
{
 protected:
  void SetUp() override
  {
    {
      auto document = MapDocument::make(kOrthogonalMapSpec);
      ASSERT_TRUE(document.has_value());
      mDocument = std::move(document.value());
    }

    const auto& registry = mDocument->get_registry();

    const auto& document_info = registry.get<CDocumentInfo>();
    mMapId = document_info.root;
  }

  [[nodiscard]]
  auto add_layer(const LayerType type) -> EntityID
  {
    CreateLayerCommand create_layer {&mDocument.value(), type};
    create_layer.redo();

    const auto& registry = mDocument->get_registry();
    const auto& map = registry.get<CMap>(mMapId);

    return map.active_layer;
  }

  std::optional<MapDocument> mDocument;
  EntityID mMapId {kInvalidEntity};
};

// tactile::core::DuplicateLayerCommand::redo
// tactile::core::DuplicateLayerCommand::undo
TEST_F(DuplicateLayerCommandTest, RedoUndoWithTopLevelLayer)
{
  auto& registry = mDocument->get_registry();
  const auto& map = registry.get<CMap>(mMapId);

  const auto source_layer_id = add_layer(LayerType::kTileLayer);
  const auto parent_layer_id = find_parent_layer(registry, map.root_layer, source_layer_id);

  ASSERT_EQ(count_layers(registry, map.root_layer), 1);

  DuplicateLayerCommand duplicate_layer {&mDocument.value(), source_layer_id};
  duplicate_layer.redo();

  const auto new_layer_id = map.active_layer;

  EXPECT_TRUE(is_layer(registry, new_layer_id));
  EXPECT_EQ(count_layers(registry, map.root_layer), 2);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, new_layer_id), parent_layer_id);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, source_layer_id), 0);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, new_layer_id), 1);

  duplicate_layer.undo();

  EXPECT_TRUE(is_layer(registry, new_layer_id));
  EXPECT_EQ(count_layers(registry, map.root_layer), 1);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, new_layer_id), std::nullopt);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, source_layer_id), 0);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, new_layer_id), std::nullopt);

  duplicate_layer.redo();

  EXPECT_TRUE(is_layer(registry, new_layer_id));
  EXPECT_EQ(count_layers(registry, map.root_layer), 2);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, new_layer_id), parent_layer_id);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, source_layer_id), 0);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, new_layer_id), 1);
}

// tactile::core::DuplicateLayerCommand::redo
// tactile::core::DuplicateLayerCommand::undo
TEST_F(DuplicateLayerCommandTest, RedoUndoWithNestedLayer)
{
  auto& registry = mDocument->get_registry();
  auto& map = registry.get<CMap>(mMapId);

  const auto group1_layer_id = add_layer(LayerType::kGroupLayer);
  const auto group2_layer_id = add_layer(LayerType::kGroupLayer);

  map.active_layer = group2_layer_id;
  const auto object_layer_id = add_layer(LayerType::kObjectLayer);

  map.active_layer = group2_layer_id;
  const auto source_layer_id = add_layer(LayerType::kTileLayer);

  const auto parent_layer_id = find_parent_layer(registry, map.root_layer, source_layer_id);

  ASSERT_EQ(count_layers(registry, map.root_layer), 4);
  ASSERT_EQ(parent_layer_id, group2_layer_id);

  DuplicateLayerCommand duplicate_layer {&mDocument.value(), source_layer_id};
  duplicate_layer.redo();

  const auto new_layer_id = map.active_layer;

  EXPECT_TRUE(is_layer(registry, new_layer_id));
  EXPECT_EQ(count_layers(registry, map.root_layer), 5);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, new_layer_id), parent_layer_id);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, source_layer_id), 1);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, new_layer_id), 2);

  duplicate_layer.undo();

  EXPECT_TRUE(is_layer(registry, new_layer_id));
  EXPECT_EQ(count_layers(registry, map.root_layer), 4);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, new_layer_id), std::nullopt);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, source_layer_id), 1);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, new_layer_id), std::nullopt);

  duplicate_layer.redo();

  EXPECT_TRUE(is_layer(registry, new_layer_id));
  EXPECT_EQ(count_layers(registry, map.root_layer), 5);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, new_layer_id), parent_layer_id);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, source_layer_id), 1);
  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, new_layer_id), 2);
}

// tactile::core::DuplicateLayerCommand::dispose
TEST_F(DuplicateLayerCommandTest, Dispose)
{
  auto& registry = mDocument->get_registry();
  const auto& map = registry.get<CMap>(mMapId);

  const auto source_layer_id = add_layer(LayerType::kObjectLayer);
  ASSERT_EQ(count_layers(registry, map.root_layer), 1);

  DuplicateLayerCommand duplicate_layer {&mDocument.value(), source_layer_id};
  duplicate_layer.redo();

  const auto layer_id = map.active_layer;
  ASSERT_TRUE(is_layer(registry, layer_id));

  duplicate_layer.dispose();
  EXPECT_TRUE(registry.is_valid(layer_id));
  EXPECT_TRUE(is_layer(registry, layer_id));

  duplicate_layer.undo();
  EXPECT_TRUE(registry.is_valid(layer_id));
  EXPECT_TRUE(is_layer(registry, layer_id));

  duplicate_layer.dispose();
  EXPECT_FALSE(registry.is_valid(layer_id));
  EXPECT_FALSE(is_layer(registry, layer_id));
}

}  // namespace
}  // namespace tactile::core
