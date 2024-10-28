// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/set_layer_visibility_command.hpp"

#include <optional>  // optional
#include <utility>   // move

#include <gtest/gtest.h>

#include "tactile/core/cmd/layer/create_layer_command.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/map/map.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class SetLayerVisibilityCommandTest : public testing::Test
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

    CreateLayerCommand create_layer {&mDocument.value(), LayerType::kTileLayer};
    create_layer.redo();

    const auto& map = registry.get<CMap>(mMapId);
    mLayerId = map.active_layer;
  }

  std::optional<MapDocument> mDocument;
  EntityID mMapId {kInvalidEntity};
  EntityID mLayerId {kInvalidEntity};
};

// tactile::core::SetLayerVisibilityCommand::redo
// tactile::core::SetLayerVisibilityCommand::undo
TEST_F(SetLayerVisibilityCommandTest, RedoUndo)
{
  const auto& registry = mDocument->get_registry();
  const auto& layer = registry.get<CLayer>(mLayerId);

  ASSERT_TRUE(layer.visible);

  SetLayerVisibilityCommand command {&mDocument.value(), mLayerId, false};

  command.redo();
  EXPECT_FALSE(layer.visible);

  command.undo();
  EXPECT_TRUE(layer.visible);

  command.redo();
  EXPECT_FALSE(layer.visible);
}

}  // namespace
}  // namespace tactile::core
