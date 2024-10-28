// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/tile/add_tileset_command.hpp"

#include <optional>  // optional
#include <utility>   // move

#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/tile/tileset.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class AddTilesetCommandTest : public testing::Test
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

  std::optional<MapDocument> mDocument;
  EntityID mMapId {kInvalidEntity};
};

// tactile::core::AddTilesetCommand::redo
// tactile::core::AddTilesetCommand::undo
TEST_F(AddTilesetCommandTest, RedoUndo)
{
  auto& registry = mDocument->get_registry();
  auto& map = registry.get<CMap>(mMapId);
  auto& id_cache = registry.get<CMapIdCache>(mMapId);

  AddTilesetCommand command {&mDocument.value(), kDummyTilesetSpec};

  ASSERT_EQ(map.attached_tilesets.size(), 0);
  ASSERT_EQ(map.active_tileset, kInvalidEntity);
  ASSERT_EQ(id_cache.next_tile_id, TileID {1});

  command.redo();

  EXPECT_EQ(map.attached_tilesets.size(), 1);
  EXPECT_NE(map.active_tileset, kInvalidEntity);
  EXPECT_TRUE(is_tileset_instance(registry, map.active_tileset));
  EXPECT_EQ(id_cache.next_tile_id, TileID {25});

  command.undo();

  EXPECT_EQ(map.attached_tilesets.size(), 0);
  EXPECT_EQ(map.active_tileset, kInvalidEntity);
  EXPECT_EQ(id_cache.next_tile_id, TileID {25});

  command.redo();

  EXPECT_EQ(map.attached_tilesets.size(), 1);
  EXPECT_NE(map.active_tileset, kInvalidEntity);
  EXPECT_TRUE(is_tileset_instance(registry, map.active_tileset));
  EXPECT_EQ(id_cache.next_tile_id, TileID {25});
}

// tactile::core::AddTilesetCommand::~AddTilesetCommand
TEST_F(AddTilesetCommandTest, UnusedTilesetShouldBeDestroyedByDestructor)
{
  auto& registry = mDocument->get_registry();

  EntityID tileset_id {kInvalidEntity};

  {
    AddTilesetCommand command {&mDocument.value(), kDummyTilesetSpec};
    command.redo();

    const auto& map = registry.get<CMap>(mMapId);
    tileset_id = map.active_tileset;
    ASSERT_TRUE(is_tileset_instance(registry, tileset_id));

    command.undo();

    EXPECT_TRUE(registry.is_valid(tileset_id));
    EXPECT_TRUE(is_tileset_instance(registry, tileset_id));
  }

  EXPECT_FALSE(registry.is_valid(tileset_id));
  EXPECT_FALSE(is_tileset_instance(registry, tileset_id));
}

}  // namespace
}  // namespace tactile::core
