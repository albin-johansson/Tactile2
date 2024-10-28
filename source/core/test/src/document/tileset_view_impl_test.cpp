// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/tileset_view_impl.hpp"

#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/io/texture.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/map/map_spec.hpp"
#include "tactile/core/tile/animation.hpp"
#include "tactile/core/tile/animation_types.hpp"
#include "tactile/core/tile/tileset.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class TilesetViewImplTest : public testing::Test
{
 public:
  TilesetViewImplTest()
    : mMapSpec {kOrthogonalMapSpec},
      mDocument {MapDocument::make(mMapSpec).value()}
  {}

 protected:
  MapSpec mMapSpec;
  MapDocument mDocument;
};

// tactile::core::TilesetViewImpl::get_first_tile_id
// tactile::core::TilesetViewImpl::tile_count
// tactile::core::TilesetViewImpl::tile_definition_count
// tactile::core::TilesetViewImpl::column_count
// tactile::core::TilesetViewImpl::get_tile_size
// tactile::core::TilesetViewImpl::get_image_size
// tactile::core::TilesetViewImpl::get_image_path
TEST_F(TilesetViewImplTest, Getters)
{
  auto& registry = mDocument.get_registry();

  const auto map_id = registry.get<CDocumentInfo>().root;
  const auto& map = registry.get<CMap>(map_id);

  add_tileset_to_map(registry, map_id, kDummyTilesetSpec).value();

  const auto tileset_id = map.attached_tilesets.back();

  const auto& tileset = registry.get<CTileset>(tileset_id);
  const auto& tileset_instance = registry.get<CTilesetInstance>(tileset_id);
  const auto& texture = registry.get<CTexture>(tileset_id);

  ASSERT_TRUE(
      add_animation_frame(registry,
                          tileset.tiles.at(0),
                          0,
                          AnimationFrame {TileIndex {0}, std::chrono::milliseconds {50}}));
  ASSERT_TRUE(
      add_animation_frame(registry,
                          tileset.tiles.at(1),
                          0,
                          AnimationFrame {TileIndex {1}, std::chrono::milliseconds {50}}));

  const TilesetViewImpl tileset_view {&mDocument, tileset_id};

  EXPECT_EQ(tileset_view.get_first_tile_id(), tileset_instance.tile_range.first_id);
  EXPECT_EQ(tileset_view.tile_count(), tileset_instance.tile_range.count);
  EXPECT_EQ(tileset_view.get_tile_size(), tileset.tile_size);

  EXPECT_EQ(tileset_view.tile_definition_count(), 2);

  EXPECT_EQ(tileset_view.column_count(), texture.size.x() / tileset.tile_size.x());
  EXPECT_EQ(tileset_view.get_image_size(), texture.size);
  EXPECT_EQ(tileset_view.get_image_path(), texture.path);
}

}  // namespace
}  // namespace tactile::core
