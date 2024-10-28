// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/tile/remove_tileset_command.hpp"

#include <optional>  // optional
#include <utility>   // move

#include <gtest/gtest.h>

#include "tactile/core/cmd/tile/add_tileset_command.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/tile/tileset.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class RemoveTilesetCommandTest : public testing::Test
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

    AddTilesetCommand add_tileset {&mDocument.value(), kDummyTilesetSpec};
    add_tileset.redo();

    const auto& map = registry.get<CMap>(mMapId);
    mTilesetId = map.active_tileset;

    ASSERT_TRUE(is_tileset_instance(registry, mTilesetId));
    ASSERT_NE(map.active_tileset, kInvalidEntity);
    ASSERT_EQ(map.attached_tilesets.size(), 1);
  }

  std::optional<MapDocument> mDocument;
  EntityID mMapId {kInvalidEntity};
  EntityID mTilesetId {kInvalidEntity};
};

// tactile::core::RemoveTilesetCommand::redo
// tactile::core::RemoveTilesetCommand::undo
TEST_F(RemoveTilesetCommandTest, RedoUndo)
{
  auto& registry = mDocument->get_registry();
  const auto& map = registry.get<CMap>(mMapId);

  RemoveTilesetCommand remove_tileset {&mDocument.value(), mTilesetId};

  remove_tileset.redo();

  EXPECT_TRUE(is_tileset_instance(registry, mTilesetId));
  EXPECT_EQ(map.active_tileset, kInvalidEntity);
  EXPECT_EQ(map.attached_tilesets.size(), 0);

  remove_tileset.undo();

  EXPECT_TRUE(is_tileset_instance(registry, mTilesetId));
  EXPECT_EQ(map.active_tileset, mTilesetId);
  EXPECT_EQ(map.attached_tilesets.size(), 1);
}

// tactile::core::RemoveTilesetCommand::~RemoveTilesetCommand
TEST_F(RemoveTilesetCommandTest, RemovedTilesetShouldBeDestroyedByDestructor)
{
  const auto& registry = mDocument->get_registry();

  {
    RemoveTilesetCommand remove_tileset {&mDocument.value(), mTilesetId};
    remove_tileset.redo();

    EXPECT_TRUE(registry.is_valid(mTilesetId));
    EXPECT_TRUE(is_tileset_instance(registry, mTilesetId));
  }

  EXPECT_FALSE(registry.is_valid(mTilesetId));
  EXPECT_FALSE(is_tileset_instance(registry, mTilesetId));
}

}  // namespace
}  // namespace tactile::core
