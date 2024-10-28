// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/tile_view_impl.hpp"

#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/document/tileset_view_impl.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/map/map_spec.hpp"
#include "tactile/core/tile/animation.hpp"
#include "tactile/core/tile/animation_types.hpp"
#include "tactile/core/tile/tile_types.hpp"
#include "tactile/core/tile/tileset.hpp"
#include "tactile/core/tile/tileset_types.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class TileViewImplTest : public testing::Test
{
 public:
  TileViewImplTest()
    : mMapSpec {kOrthogonalMapSpec},
      mDocument {MapDocument::make(mMapSpec).value()}
  {}

 protected:
  MapSpec mMapSpec;
  MapDocument mDocument;
};

// tactile::core::TileViewImpl::get_parent_tileset
// tactile::core::TileViewImpl::get_index
// tactile::core::TileViewImpl::object_count
// tactile::core::TileViewImpl::animation_frame_count
// tactile::core::TileViewImpl::get_animation_frame
TEST_F(TileViewImplTest, Getters)
{
  auto& registry = mDocument.get_registry();

  const auto map_id = registry.get<CDocumentInfo>().root;
  auto& map = registry.get<CMap>(map_id);

  add_tileset_to_map(registry, map_id, kDummyTilesetSpec).value();

  const auto tileset_id = map.attached_tilesets.back();
  const auto& tileset = registry.get<CTileset>(tileset_id);

  const auto tile_id = tileset.tiles.front();
  const auto& tile = registry.get<CTile>(tile_id);

  add_animation_frame(registry,
                      tile_id,
                      0,
                      AnimationFrame {tile.index, std::chrono::milliseconds {1}})
      .value();
  add_animation_frame(registry,
                      tile_id,
                      1,
                      AnimationFrame {tile.index + 1, std::chrono::milliseconds {2}})
      .value();

  const auto& animation = registry.get<CAnimation>(tile_id);

  const TilesetViewImpl tileset_view {&mDocument, tileset_id};
  const TileViewImpl tile_view {&mDocument, &tileset_view, tile_id};

  EXPECT_EQ(&tile_view.get_parent_tileset(), &tileset_view);
  EXPECT_EQ(tile_view.get_index(), tile.index);
  EXPECT_EQ(tile_view.object_count(), tile.objects.size());
  EXPECT_EQ(tile_view.animation_frame_count(), animation.frames.size());

  EXPECT_EQ(tile_view.get_animation_frame(0).first, tile.index);
  EXPECT_EQ(tile_view.get_animation_frame(1).first, tile.index + 1);
  EXPECT_EQ(tile_view.get_animation_frame(0).second, std::chrono::milliseconds {1});
  EXPECT_EQ(tile_view.get_animation_frame(1).second, std::chrono::milliseconds {2});

  EXPECT_ANY_THROW((void) tile_view.get_animation_frame(tile_view.animation_frame_count()));
}

}  // namespace
}  // namespace tactile::core
