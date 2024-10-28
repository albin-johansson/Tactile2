// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/move_layer_down_command.hpp"

#include <optional>  // optional
#include <utility>   // move

#include <gtest/gtest.h>

#include "tactile/core/cmd/layer/create_layer_command.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/map/map.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class MoveLayerDownCommandTest : public testing::Test
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
    mMapId = registry.get<CDocumentInfo>().root;

    const auto& map = registry.get<CMap>(mMapId);

    CreateLayerCommand {&mDocument.value(), LayerType::kGroupLayer}.redo();

    mLayerId = map.active_layer;

    CreateLayerCommand {&mDocument.value(), LayerType::kGroupLayer}.redo();
    CreateLayerCommand {&mDocument.value(), LayerType::kTileLayer}.redo();
    CreateLayerCommand {&mDocument.value(), LayerType::kObjectLayer}.redo();

    ASSERT_EQ(count_layers(registry, map.root_layer), 4);
  }

  std::optional<MapDocument> mDocument;
  EntityID mMapId {kInvalidEntity};
  EntityID mLayerId {kInvalidEntity};
};

// tactile::core::MoveLayerDownCommand::redo
// tactile::core::MoveLayerDownCommand::undo
TEST_F(MoveLayerDownCommandTest, RedoUndo)
{
  auto& registry = mDocument->get_registry();
  const auto& map = registry.get<CMap>(mMapId);

  ASSERT_EQ(get_local_layer_index(registry, map.root_layer, mLayerId), 0);
  ASSERT_TRUE(can_move_layer_down(registry, map.root_layer, mLayerId));

  MoveLayerDownCommand move_down {&mDocument.value(), mLayerId};
  move_down.redo();

  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, mLayerId), 1);
  EXPECT_FALSE(can_move_layer_down(registry, map.root_layer, mLayerId));

  move_down.undo();

  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, mLayerId), 0);
  EXPECT_TRUE(can_move_layer_down(registry, map.root_layer, mLayerId));
}

}  // namespace
}  // namespace tactile::core
