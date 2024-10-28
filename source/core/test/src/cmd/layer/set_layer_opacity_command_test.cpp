// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/set_layer_opacity_command.hpp"

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

class SetLayerOpacityCommandTest : public testing::Test
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

// tactile::core::SetLayerOpacityCommand::redo
// tactile::core::SetLayerOpacityCommand::undo
TEST_F(SetLayerOpacityCommandTest, RedoUndo)
{
  constexpr float old_opacity = 0.8f;
  constexpr float new_opacity = 0.5f;

  auto& registry = mDocument->get_registry();
  auto& layer = registry.get<CLayer>(mLayerId);
  layer.opacity = old_opacity;

  SetLayerOpacityCommand set_opacity {&mDocument.value(), mLayerId, new_opacity};
  EXPECT_EQ(layer.opacity, old_opacity);

  set_opacity.redo();
  EXPECT_EQ(layer.opacity, new_opacity);

  set_opacity.undo();
  EXPECT_EQ(layer.opacity, old_opacity);

  set_opacity.redo();
  EXPECT_EQ(layer.opacity, new_opacity);
}

// tactile::core::SetLayerOpacityCommand::merge_with
TEST_F(SetLayerOpacityCommandTest, MergeWith)
{
  constexpr float opacity1 = 0.1f;
  constexpr float opacity2 = 0.2f;
  constexpr float opacity3 = 0.3f;

  auto& registry = mDocument->get_registry();
  auto& layer = registry.get<CLayer>(mLayerId);
  layer.opacity = opacity1;

  SetLayerOpacityCommand set_opacity1 {&mDocument.value(), mLayerId, opacity1};
  const SetLayerOpacityCommand set_opacity2 {&mDocument.value(), mLayerId, opacity2};
  const SetLayerOpacityCommand set_opacity3 {&mDocument.value(), mLayerId, opacity3};

  ASSERT_EQ(layer.opacity, opacity1);
  EXPECT_TRUE(set_opacity1.merge_with(&set_opacity2));

  set_opacity1.redo();
  EXPECT_EQ(layer.opacity, opacity2);

  set_opacity1.undo();
  EXPECT_EQ(layer.opacity, opacity1);

  EXPECT_TRUE(set_opacity1.merge_with(&set_opacity3));

  set_opacity1.redo();
  EXPECT_EQ(layer.opacity, opacity3);

  set_opacity1.undo();
  EXPECT_EQ(layer.opacity, opacity1);
}

}  // namespace
}  // namespace tactile::core
