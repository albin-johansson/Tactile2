// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/create_layer_command.hpp"

#include <optional>  // optional
#include <utility>   // move

#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer.hpp"
#include "tactile/core/layer/object_layer.hpp"
#include "tactile/core/layer/tile_layer.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/tile/tileset.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class CreateLayerCommandTest : public testing::Test
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

  std::optional<MapDocument> mDocument;
  EntityID mMapId {kInvalidEntity};
};

// tactile::core::CreateLayerCommand::redo
// tactile::core::CreateLayerCommand::undo
TEST_F(CreateLayerCommandTest, RedoUndoWithNoActiveLayer)
{
  auto& registry = mDocument->get_registry();
  const auto& map = registry.get<CMap>(mMapId);

  ASSERT_EQ(count_layers(registry, map.root_layer), 0);

  CreateLayerCommand create_layer {&mDocument.value(), LayerType::kTileLayer};
  create_layer.redo();

  const auto layer_id = map.active_layer;
  ASSERT_TRUE(is_tile_layer(registry, layer_id));

  EXPECT_EQ(count_layers(registry, map.root_layer), 1);
  EXPECT_EQ(get_global_layer_index(registry, map.root_layer, layer_id), 0);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), map.root_layer);

  create_layer.undo();

  EXPECT_EQ(count_layers(registry, map.root_layer), 0);
  EXPECT_EQ(get_global_layer_index(registry, map.root_layer, layer_id), std::nullopt);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), std::nullopt);

  create_layer.redo();

  EXPECT_EQ(count_layers(registry, map.root_layer), 1);
  EXPECT_EQ(get_global_layer_index(registry, map.root_layer, layer_id), 0);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), map.root_layer);
}

// tactile::core::CreateLayerCommand::redo
// tactile::core::CreateLayerCommand::undo
TEST_F(CreateLayerCommandTest, RedoUndoWithActiveGroupLayer)
{
  auto& registry = mDocument->get_registry();
  auto& map = registry.get<CMap>(mMapId);

  ASSERT_EQ(count_layers(registry, map.root_layer), 0);

  {
    CreateLayerCommand add_group_layer {&mDocument.value(), LayerType::kGroupLayer};
    add_group_layer.redo();
  }

  const auto group_layer_id = map.active_layer;
  ASSERT_EQ(count_layers(registry, map.root_layer), 1);
  ASSERT_TRUE(is_group_layer(registry, group_layer_id));

  CreateLayerCommand add_object_layer {&mDocument.value(), LayerType::kObjectLayer};
  add_object_layer.redo();

  const auto layer_id = map.active_layer;
  ASSERT_TRUE(is_object_layer(registry, layer_id));

  EXPECT_EQ(count_layers(registry, map.root_layer), 2);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), group_layer_id);
  EXPECT_EQ(get_global_layer_index(registry, map.root_layer, group_layer_id), 0);
  EXPECT_EQ(get_global_layer_index(registry, map.root_layer, layer_id), 1);

  add_object_layer.undo();

  EXPECT_EQ(count_layers(registry, map.root_layer), 1);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), std::nullopt);
  EXPECT_EQ(get_global_layer_index(registry, map.root_layer, group_layer_id), 0);
  EXPECT_EQ(get_global_layer_index(registry, map.root_layer, layer_id), std::nullopt);

  add_object_layer.redo();

  EXPECT_EQ(count_layers(registry, map.root_layer), 2);
  EXPECT_EQ(find_parent_layer(registry, map.root_layer, layer_id), group_layer_id);
  EXPECT_EQ(get_global_layer_index(registry, map.root_layer, group_layer_id), 0);
  EXPECT_EQ(get_global_layer_index(registry, map.root_layer, layer_id), 1);
}

// tactile::core::CreateLayerCommand::dispose
TEST_F(CreateLayerCommandTest, Dispose)
{
  auto& registry = mDocument->get_registry();
  const auto& map = registry.get<CMap>(mMapId);

  CreateLayerCommand add_layer {&mDocument.value(), LayerType::kTileLayer};
  add_layer.redo();

  const auto layer_id = map.active_layer;
  ASSERT_TRUE(is_layer(registry, layer_id));

  add_layer.dispose();
  EXPECT_TRUE(registry.is_valid(layer_id));
  EXPECT_TRUE(is_layer(registry, layer_id));

  add_layer.undo();
  EXPECT_TRUE(registry.is_valid(layer_id));
  EXPECT_TRUE(is_layer(registry, layer_id));

  add_layer.dispose();
  EXPECT_FALSE(registry.is_valid(layer_id));
  EXPECT_FALSE(is_layer(registry, layer_id));
}

}  // namespace
}  // namespace tactile::core
