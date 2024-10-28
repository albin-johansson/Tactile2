// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/move_layer_up_command.hpp"

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

class MoveLayerUpCommandTest : public testing::Test
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

    CreateLayerCommand {&mDocument.value(), LayerType::kGroupLayer}.redo();
    CreateLayerCommand {&mDocument.value(), LayerType::kTileLayer}.redo();
    CreateLayerCommand {&mDocument.value(), LayerType::kObjectLayer}.redo();

    const auto& map = registry.get<CMap>(mMapId);
    ASSERT_EQ(count_layers(registry, map.root_layer), 3);

    mLayerId = map.active_layer;
  }

  std::optional<MapDocument> mDocument;
  EntityID mMapId {kInvalidEntity};
  EntityID mLayerId {kInvalidEntity};
};

// tactile::core::MoveLayerUpCommand::redo
// tactile::core::MoveLayerUpCommand::undo
TEST_F(MoveLayerUpCommandTest, RedoUndo)
{
  auto& registry = mDocument->get_registry();
  const auto& map = registry.get<CMap>(mMapId);

  ASSERT_EQ(get_local_layer_index(registry, map.root_layer, mLayerId), 1);
  ASSERT_TRUE(can_move_layer_up(registry, map.root_layer, mLayerId));

  MoveLayerUpCommand move_up {&mDocument.value(), mLayerId};
  move_up.redo();

  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, mLayerId), 0);
  EXPECT_FALSE(can_move_layer_up(registry, map.root_layer, mLayerId));

  move_up.undo();

  EXPECT_EQ(get_local_layer_index(registry, map.root_layer, mLayerId), 1);
  EXPECT_TRUE(can_move_layer_up(registry, map.root_layer, mLayerId));
}

}  // namespace
}  // namespace tactile::core
