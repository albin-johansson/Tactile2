// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/map/map.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/map/map_spec.hpp"
#include "tactile/core/meta/meta.hpp"
#include "tactile/core/test/ir_comparison.hpp"
#include "tactile/core/tile/tileset.hpp"
#include "tactile/core/tile/tileset_types.hpp"
#include "tactile/core/ui/viewport.hpp"
#include "tactile/null_renderer/null_renderer.hpp"
#include "tactile/test_util/ir.hpp"
#include "tactile/test_util/ir_presets.hpp"

namespace tactile::core {
namespace {

class MapTest : public testing::Test
{
 public:
  MapTest()
  {
    mRegistry.add<CTileCache>();
  }

 protected:
  Registry mRegistry {};
  null_renderer::NullRenderer mRenderer {nullptr};
};

// tactile::core::is_map
TEST_F(MapTest, IsMap)
{
  const MapSpec spec {
    .orientation = TileOrientation::kOrthogonal,
    .extent = Extent2D {5, 5},
    .tile_size = Int2 {16, 16},
  };

  const auto map_entity = make_map(mRegistry, spec);

  EXPECT_FALSE(is_map(mRegistry, kInvalidEntity));
  EXPECT_FALSE(is_map(mRegistry, mRegistry.make_entity()));
  EXPECT_TRUE(is_map(mRegistry, map_entity));
}

// tactile::core::make_map [Registry&, const MapSpec&]
TEST_F(MapTest, MakeMap)
{
  constexpr MapSpec spec {
    .orientation = TileOrientation::kOrthogonal,
    .extent = Extent2D {10, 8},
    .tile_size = Int2 {32, 36},
  };

  const auto map_entity = make_map(mRegistry, spec);
  ASSERT_TRUE(is_map(mRegistry, map_entity));

  ASSERT_TRUE(mRegistry.has<CMeta>(map_entity));
  ASSERT_TRUE(mRegistry.has<CMap>(map_entity));
  ASSERT_TRUE(mRegistry.has<CTileFormat>(map_entity));
  ASSERT_TRUE(mRegistry.has<CMapIdCache>(map_entity));
  ASSERT_TRUE(mRegistry.has<CLayerSuffixes>(map_entity));
  ASSERT_TRUE(mRegistry.has<CViewport>(map_entity));

  const auto& meta = mRegistry.get<CMeta>(map_entity);
  const auto& map = mRegistry.get<CMap>(map_entity);
  const auto& format = mRegistry.get<CTileFormat>(map_entity);
  const auto& id_cache = mRegistry.get<CMapIdCache>(map_entity);
  const auto& suffixes = mRegistry.get<CLayerSuffixes>(map_entity);
  const auto& viewport = mRegistry.get<CViewport>(map_entity);

  EXPECT_EQ(meta.name, "");
  EXPECT_EQ(meta.properties.size(), 0);
  EXPECT_EQ(meta.components.size(), 0);

  EXPECT_EQ(map.orientation, spec.orientation);
  EXPECT_EQ(map.extent, spec.extent);
  EXPECT_EQ(map.tile_size, spec.tile_size);
  EXPECT_NE(map.root_layer, kInvalidEntity);
  EXPECT_EQ(map.active_layer, kInvalidEntity);
  EXPECT_EQ(map.active_tileset, kInvalidEntity);
  EXPECT_TRUE(is_group_layer(mRegistry, map.root_layer));

  EXPECT_EQ(format.encoding, TileEncoding::kPlainText);
  EXPECT_EQ(format.compression, std::nullopt);
  EXPECT_EQ(format.comp_level, std::nullopt);

  EXPECT_EQ(id_cache.next_tile_id, TileID {1});
  EXPECT_EQ(id_cache.next_layer_id, 1);
  EXPECT_EQ(id_cache.next_object_id, 1);

  EXPECT_EQ(suffixes.tile_layer_suffix, 1);
  EXPECT_EQ(suffixes.object_layer_suffix, 1);
  EXPECT_EQ(suffixes.group_layer_suffix, 1);

  EXPECT_EQ(viewport.pos, Float2 {});
  EXPECT_EQ(viewport.size, Float2 {});
  EXPECT_EQ(viewport.scale, 1.0f);
}

// tactile::core::make_map [Registry&, IRenderer&, const ir::Map&]
TEST_F(MapTest, MakeMapFromIR)
{
  const auto ir_map = test::make_complex_ir_map(test::make_ir_tile_format());

  const auto map_id = make_map(mRegistry, mRenderer, ir_map);
  ASSERT_TRUE(map_id.has_value());
  ASSERT_TRUE(is_map(mRegistry, *map_id));

  compare_map(mRegistry, *map_id, ir_map);
}

// tactile::core::make_map
TEST_F(MapTest, MakeMapWithInvalidSpec)
{
  constexpr MapSpec bad_extent {
    .orientation = TileOrientation::kOrthogonal,
    .extent = Extent2D {0, 0},
    .tile_size = Int2 {32, 32},
  };

  constexpr MapSpec bad_tile_size {
    .orientation = TileOrientation::kOrthogonal,
    .extent = Extent2D {10, 10},
    .tile_size = Int2 {0, 0},
  };

  EXPECT_EQ(make_map(mRegistry, bad_extent), kInvalidEntity);
  EXPECT_EQ(make_map(mRegistry, bad_tile_size), kInvalidEntity);
}

// tactile::core::destroy_map
TEST_F(MapTest, DestroyMap)
{
  constexpr MapSpec spec {
    .orientation = TileOrientation::kOrthogonal,
    .extent = Extent2D {5, 5},
    .tile_size = Int2 {32, 32},
  };

  const auto map_entity = make_map(mRegistry, spec);

  EXPECT_EQ(mRegistry.count<CMeta>(), 2);
  EXPECT_EQ(mRegistry.count<CMap>(), 1);
  EXPECT_EQ(mRegistry.count<CTileFormat>(), 1);
  EXPECT_EQ(mRegistry.count<CMapIdCache>(), 1);
  EXPECT_EQ(mRegistry.count<CLayerSuffixes>(), 1);
  EXPECT_EQ(mRegistry.count<CViewport>(), 1);
  EXPECT_EQ(mRegistry.count<CGroupLayer>(), 1);
  EXPECT_EQ(mRegistry.count<CLayer>(), 1);
  EXPECT_EQ(mRegistry.count(), 9);

  destroy_map(mRegistry, map_entity);

  EXPECT_EQ(mRegistry.count<CMeta>(), 0);
  EXPECT_EQ(mRegistry.count<CMap>(), 0);
  EXPECT_EQ(mRegistry.count<CTileFormat>(), 0);
  EXPECT_EQ(mRegistry.count<CMapIdCache>(), 0);
  EXPECT_EQ(mRegistry.count<CLayerSuffixes>(), 0);
  EXPECT_EQ(mRegistry.count<CViewport>(), 0);
  EXPECT_EQ(mRegistry.count<CGroupLayer>(), 0);
  EXPECT_EQ(mRegistry.count<CLayer>(), 0);
  EXPECT_EQ(mRegistry.count(), 0);
}

// tactile::core::add_tileset_to_map
TEST_F(MapTest, AddTilesetToMap)
{
  constexpr MapSpec spec {
    .orientation = TileOrientation::kOrthogonal,
    .extent = Extent2D {10, 10},
    .tile_size = Int2 {32, 32},
  };

  const auto map_entity = make_map(mRegistry, spec);

  const auto& map = mRegistry.get<CMap>(map_entity);
  const auto& id_cache = mRegistry.get<CMapIdCache>(map_entity);

  const TilesetSpec tileset_spec {
    .tile_size = Int2 {50, 50},
    .texture =
        CTexture {
          .raw_handle = nullptr,
          .id = TextureID {42},
          .size = Int2 {500, 500},
          .path = "foo/bar.png",
        },
  };

  EXPECT_EQ(map.active_tileset, kInvalidEntity);
  EXPECT_EQ(map.attached_tilesets.size(), 0);
  EXPECT_EQ(id_cache.next_tile_id, TileID {1});
  EXPECT_EQ(id_cache.next_layer_id, 1);
  EXPECT_EQ(id_cache.next_object_id, 1);
  EXPECT_EQ(mRegistry.count<CTileset>(), 0);
  EXPECT_EQ(mRegistry.count<CTilesetInstance>(), 0);

  EXPECT_TRUE(add_tileset_to_map(mRegistry, map_entity, tileset_spec).has_value());

  EXPECT_NE(map.active_tileset, kInvalidEntity);
  EXPECT_EQ(map.attached_tilesets.size(), 1);
  EXPECT_EQ(id_cache.next_tile_id, TileID {101});
  EXPECT_EQ(id_cache.next_layer_id, 1);
  EXPECT_EQ(id_cache.next_object_id, 1);
  EXPECT_EQ(mRegistry.count<CTileset>(), 1);
  EXPECT_EQ(mRegistry.count<CTilesetInstance>(), 1);
}

}  // namespace
}  // namespace tactile::core
