// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/remove_layer_command.hpp"

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

class RemoveLayerCommandTest : public testing::Test
{
 protected:
  void SetUp() override
  {
    {
      auto document = MapDocument::make(kOrthogonalMapSpec);
      ASSERT_TRUE(document.has_value());
      mDocument = std::move(document.value());
    }

    auto& registry = mDocument->get_registry();

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

// tactile::core::RemoveLayerCommand::redo
// tactile::core::RemoveLayerCommand::undo
TEST_F(RemoveLayerCommandTest, RedoUndoWithTopLevelLayer)
{
  auto& registry = mDocument->get_registry();
  const auto& map = registry.get<CMap>(mMapId);

  const auto layer_id = add_layer(LayerType::kTileLayer);

  RemoveLayerCommand remove_layer {&mDocument.value(), layer_id};
  remove_layer.redo();

  EXPECT_TRUE(is_layer(registry, layer_id));
  EXPECT_EQ(map.active_layer, kInvalidEntity);
  EXPECT_EQ(count_layers(registry, map.root_layer), 0);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), std::nullopt);

  remove_layer.undo();

  EXPECT_TRUE(is_layer(registry, layer_id));
  EXPECT_EQ(map.active_layer, layer_id);
  EXPECT_EQ(count_layers(registry, map.root_layer), 1);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), map.root_layer);

  remove_layer.redo();

  EXPECT_TRUE(is_layer(registry, layer_id));
  EXPECT_EQ(map.active_layer, kInvalidEntity);
  EXPECT_EQ(count_layers(registry, map.root_layer), 0);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), std::nullopt);
}

// tactile::core::RemoveLayerCommand::redo
// tactile::core::RemoveLayerCommand::undo
TEST_F(RemoveLayerCommandTest, RedoUndoWithNestedLayer)
{
  auto& registry = mDocument->get_registry();
  const auto& map = registry.get<CMap>(mMapId);

  const auto group_layer_id = add_layer(LayerType::kGroupLayer);
  const auto layer_id = add_layer(LayerType::kObjectLayer);

  EXPECT_TRUE(is_layer(registry, layer_id));
  EXPECT_EQ(map.active_layer, layer_id);
  EXPECT_EQ(count_layers(registry, map.root_layer), 2);
  ASSERT_EQ(find_parent_layer(registry, map.root_layer, layer_id), group_layer_id);

  RemoveLayerCommand remove_layer {&mDocument.value(), layer_id};
  remove_layer.redo();

  EXPECT_TRUE(is_layer(registry, layer_id));
  EXPECT_EQ(map.active_layer, kInvalidEntity);
  EXPECT_EQ(count_layers(registry, map.root_layer), 1);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), std::nullopt);

  remove_layer.undo();

  EXPECT_TRUE(is_layer(registry, layer_id));
  EXPECT_EQ(map.active_layer, layer_id);
  EXPECT_EQ(count_layers(registry, map.root_layer), 2);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), group_layer_id);

  remove_layer.redo();

  EXPECT_TRUE(is_layer(registry, layer_id));
  EXPECT_EQ(map.active_layer, kInvalidEntity);
  EXPECT_EQ(count_layers(registry, map.root_layer), 1);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), std::nullopt);
}

// tactile::core::RemoveLayerCommand::dispose
TEST_F(RemoveLayerCommandTest, Dispose)
{
  const auto& registry = mDocument->get_registry();
  const auto layer_id = add_layer(LayerType::kTileLayer);

  RemoveLayerCommand remove_layer {&mDocument.value(), layer_id};

  remove_layer.redo();
  EXPECT_TRUE(registry.is_valid(layer_id));
  EXPECT_TRUE(is_layer(registry, layer_id));

  remove_layer.undo();
  remove_layer.dispose();
  EXPECT_TRUE(registry.is_valid(layer_id));
  EXPECT_TRUE(is_layer(registry, layer_id));

  remove_layer.redo();
  remove_layer.dispose();
  EXPECT_FALSE(registry.is_valid(layer_id));
  EXPECT_FALSE(is_layer(registry, layer_id));
}

}  // namespace
}  // namespace tactile::core
