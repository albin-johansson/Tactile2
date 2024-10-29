// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/tile/tileset.hpp"

#include <gtest/gtest.h>

#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/io/texture.hpp"
#include "tactile/core/meta/meta.hpp"
#include "tactile/core/test/ir_comparison.hpp"
#include "tactile/core/tile/animation.hpp"
#include "tactile/core/tile/animation_types.hpp"
#include "tactile/core/tile/tile.hpp"
#include "tactile/core/tile/tile_types.hpp"
#include "tactile/core/tile/tileset_types.hpp"
#include "tactile/core/ui/viewport.hpp"
#include "tactile/null_renderer/null_renderer.hpp"
#include "tactile/test_util/ir_presets.hpp"

namespace tactile::core {
namespace {

class TilesetTest : public testing::Test
{
 public:
  TilesetTest()
  {
    mRegistry.add<CTileCache>();
  }

  [[nodiscard]]
  static auto make_dummy_texture(const Int2 size) -> CTexture
  {
    CTexture texture {};

    texture.id = TextureID {10};
    texture.size = size;
    texture.path = "foo/bar.png";

    return texture;
  }

  [[nodiscard]]
  auto make_dummy_tileset_with_100_tiles() -> EntityID
  {
    const auto texture = make_dummy_texture(Int2 {100, 100});

    const TilesetSpec spec {
      .tile_size = Int2 {10, 10},
      .texture = texture,
    };

    return make_tileset(mRegistry, spec);
  }

 protected:
  Registry mRegistry {};
  NullRenderer mRenderer {nullptr};
};

// tactile::core::make_tileset [Registry&, const TilesetSpec&]
TEST_F(TilesetTest, MakeTileset)
{
  const TilesetSpec spec {.tile_size = Int2 {50, 30},
                          .texture = make_dummy_texture(Int2(600, 330))};

  const auto ts_entity = make_tileset(mRegistry, spec);

  EXPECT_TRUE(mRegistry.is_valid(ts_entity));
  EXPECT_TRUE(is_tileset(mRegistry, ts_entity));

  const auto& meta = mRegistry.get<CMeta>(ts_entity);
  const auto& tileset = mRegistry.get<CTileset>(ts_entity);
  const auto& texture = mRegistry.get<CTexture>(ts_entity);
  const auto& viewport = mRegistry.get<CViewport>(ts_entity);

  EXPECT_EQ(meta.name, "");
  EXPECT_EQ(meta.properties.size(), 0);
  EXPECT_EQ(meta.components.size(), 0);

  EXPECT_EQ(tileset.tile_size, spec.tile_size);
  EXPECT_EQ(tileset.extent.rows, spec.texture.size.y() / spec.tile_size.y());
  EXPECT_EQ(tileset.extent.cols, spec.texture.size.x() / spec.tile_size.x());
  EXPECT_EQ(tileset.tiles.size(), 132);
  EXPECT_EQ(tileset.tiles.size(), tileset.extent.rows * tileset.extent.cols);

  for (const auto tile_entity : tileset.tiles) {
    EXPECT_TRUE(mRegistry.is_valid(ts_entity));
    EXPECT_TRUE(is_tile(mRegistry, tile_entity));
  }

  EXPECT_EQ(texture.raw_handle, spec.texture.raw_handle);
  EXPECT_EQ(texture.id, spec.texture.id);
  EXPECT_EQ(texture.size, spec.texture.size);
  EXPECT_EQ(texture.path, spec.texture.path);

  EXPECT_GT(viewport.size.x(), 0.0f);
  EXPECT_GT(viewport.size.y(), 0.0f);
  EXPECT_EQ(viewport.pos.x(), 0.0f);
  EXPECT_EQ(viewport.pos.y(), 0.0f);
}

// tactile::core::make_tileset [Registry&, IRenderer&, const ir::TilesetRef&]
TEST_F(TilesetTest, MakeTilesetFromIR)
{
  TileID next_tile_id {1000};
  ObjectID next_object_id {10};
  const auto ir_tileset_ref = test::make_complex_ir_tileset(next_tile_id, next_object_id);

  const auto tileset_id = make_tileset(mRegistry, mRenderer, ir_tileset_ref);
  ASSERT_TRUE(tileset_id.has_value());

  compare_tileset(mRegistry, *tileset_id, ir_tileset_ref);
}

// tactile::core::init_tileset_instance
TEST_F(TilesetTest, InitTilesetInstance)
{
  const auto& tile_cache = mRegistry.get<CTileCache>();
  ASSERT_EQ(tile_cache.tileset_mapping.size(), 0);

  const auto ts_entity = make_dummy_tileset_with_100_tiles();
  ASSERT_FALSE(mRegistry.has<CTilesetInstance>(ts_entity));

  // [42, 142)
  const TileID first_tile {42};
  ASSERT_TRUE(init_tileset_instance(mRegistry, ts_entity, first_tile).has_value());
  ASSERT_TRUE(mRegistry.has<CTilesetInstance>(ts_entity));

  const auto& tileset = mRegistry.get<CTileset>(ts_entity);
  const auto& instance = mRegistry.get<CTilesetInstance>(ts_entity);

  EXPECT_EQ(instance.tile_range.first_id, first_tile);
  EXPECT_EQ(instance.tile_range.count, saturate_cast<std::int32_t>(tileset.tiles.size()));
  EXPECT_FALSE(instance.is_embedded);

  EXPECT_EQ(tile_cache.tileset_mapping.size(), tileset.tiles.size());

  EXPECT_FALSE(tile_cache.tileset_mapping.contains(first_tile - TileID {1}));
  EXPECT_FALSE(tile_cache.tileset_mapping.contains(first_tile + instance.tile_range.count));

  for (std::int32_t index = 0; index < instance.tile_range.count; ++index) {
    const TileID tile_id {instance.tile_range.first_id + index};
    EXPECT_TRUE(tile_cache.tileset_mapping.contains(tile_id));
  }
}

// tactile::core::init_tileset_instance
TEST_F(TilesetTest, InitTilesetInstanceWithInvalidTileIdentifier)
{
  const auto ts_entity = make_dummy_tileset_with_100_tiles();
  EXPECT_FALSE(init_tileset_instance(mRegistry, ts_entity, kEmptyTile).has_value());
  EXPECT_FALSE(init_tileset_instance(mRegistry, ts_entity, TileID {-1}).has_value());
  EXPECT_EQ(mRegistry.count<CTilesetInstance>(), 0);
}

// tactile::core::init_tileset_instance
TEST_F(TilesetTest, InitTilesetInstanceMoreThanOnce)
{
  const auto ts_entity = make_dummy_tileset_with_100_tiles();
  EXPECT_TRUE(init_tileset_instance(mRegistry, ts_entity, TileID {1}).has_value());
  EXPECT_FALSE(init_tileset_instance(mRegistry, ts_entity, TileID {101}).has_value());
}

// tactile::core::init_tileset_instance
TEST_F(TilesetTest, InitTilesetInstanceTileRangeCollisionDetection)
{
  const auto& tile_cache = mRegistry.get<CTileCache>();
  ASSERT_EQ(tile_cache.tileset_mapping.size(), 0);

  const auto ts1_entity = make_dummy_tileset_with_100_tiles();
  const auto ts2_entity = make_dummy_tileset_with_100_tiles();

  // [1, 101)
  EXPECT_TRUE(init_tileset_instance(mRegistry, ts1_entity, TileID {1}).has_value());

  // These tile ranges would overlap with the existing tileset.
  EXPECT_FALSE(init_tileset_instance(mRegistry, ts2_entity, TileID {1}).has_value());
  EXPECT_FALSE(init_tileset_instance(mRegistry, ts2_entity, TileID {50}).has_value());
  EXPECT_FALSE(init_tileset_instance(mRegistry, ts2_entity, TileID {100}).has_value());

  // [101, 201)
  EXPECT_TRUE(init_tileset_instance(mRegistry, ts2_entity, TileID {101}).has_value());
}

// tactile::core::destroy_tileset
TEST_F(TilesetTest, DestroyTileset)
{
  const auto ts_entity = make_dummy_tileset_with_100_tiles();

  EXPECT_TRUE(mRegistry.is_valid(ts_entity));
  EXPECT_EQ(mRegistry.count<CMeta>(), 101);
  EXPECT_EQ(mRegistry.count<CTileset>(), 1);
  EXPECT_EQ(mRegistry.count<CTilesetInstance>(), 0);
  EXPECT_EQ(mRegistry.count<CTexture>(), 1);
  EXPECT_EQ(mRegistry.count<CTile>(), 100);
  EXPECT_GT(mRegistry.count(), 0);

  destroy_tileset(mRegistry, ts_entity);

  EXPECT_FALSE(mRegistry.is_valid(ts_entity));
  EXPECT_EQ(mRegistry.count<CMeta>(), 0);
  EXPECT_EQ(mRegistry.count<CTileset>(), 0);
  EXPECT_EQ(mRegistry.count<CTilesetInstance>(), 0);
  EXPECT_EQ(mRegistry.count<CTexture>(), 0);
  EXPECT_EQ(mRegistry.count<CTile>(), 0);
  EXPECT_EQ(mRegistry.count(), 0);
}

// tactile::core::destroy_tileset
TEST_F(TilesetTest, DestroyTilesetInstance)
{
  const auto ts_entity = make_dummy_tileset_with_100_tiles();

  const TileID first_tile {25};
  ASSERT_TRUE(init_tileset_instance(mRegistry, ts_entity, first_tile).has_value());

  const auto& tile_cache = mRegistry.get<CTileCache>();

  EXPECT_TRUE(mRegistry.is_valid(ts_entity));
  EXPECT_EQ(mRegistry.count<CMeta>(), 101);
  EXPECT_EQ(mRegistry.count<CTileset>(), 1);
  EXPECT_EQ(mRegistry.count<CTilesetInstance>(), 1);
  EXPECT_EQ(mRegistry.count<CTexture>(), 1);
  EXPECT_EQ(mRegistry.count<CTile>(), 100);
  EXPECT_GT(mRegistry.count(), 0);
  EXPECT_EQ(tile_cache.tileset_mapping.size(), 100);

  destroy_tileset(mRegistry, ts_entity);

  EXPECT_FALSE(mRegistry.is_valid(ts_entity));
  EXPECT_EQ(mRegistry.count<CMeta>(), 0);
  EXPECT_EQ(mRegistry.count<CTileset>(), 0);
  EXPECT_EQ(mRegistry.count<CTilesetInstance>(), 0);
  EXPECT_EQ(mRegistry.count<CTexture>(), 0);
  EXPECT_EQ(mRegistry.count<CTile>(), 0);
  EXPECT_EQ(mRegistry.count(), 0);
  EXPECT_EQ(tile_cache.tileset_mapping.size(), 0);
}

// tactile::core::get_tile_appearance
TEST_F(TilesetTest, GetTileAppearance)
{
  const auto ts_entity = make_dummy_tileset_with_100_tiles();
  const auto& tileset = mRegistry.get<CTileset>(ts_entity);

  const TileIndex index10 {10};
  const TileIndex index11 {11};
  const TileIndex index12 {12};

  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index10), index10);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index11), index11);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index12), index12);

  const auto tile10_entity = tileset.tiles.at(10);
  ASSERT_TRUE(add_animation_frame(mRegistry,
                                  tile10_entity,
                                  0,
                                  AnimationFrame {index10, std::chrono::milliseconds::zero()})
                  .has_value());
  ASSERT_TRUE(add_animation_frame(mRegistry,
                                  tile10_entity,
                                  1,
                                  AnimationFrame {index11, std::chrono::milliseconds::zero()})
                  .has_value());
  ASSERT_TRUE(add_animation_frame(mRegistry,
                                  tile10_entity,
                                  2,
                                  AnimationFrame {index12, std::chrono::milliseconds::zero()})
                  .has_value());

  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index10), index10);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index11), index11);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index12), index12);

  update_animations(mRegistry);

  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index10), index11);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index11), index11);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index12), index12);

  update_animations(mRegistry);

  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index10), index12);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index11), index11);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index12), index12);

  update_animations(mRegistry);

  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index10), index10);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index11), index11);
  EXPECT_EQ(get_tile_appearance(mRegistry, ts_entity, index12), index12);
}

// tactile::core::find_tileset
TEST_F(TilesetTest, FindTileset)
{
  EXPECT_EQ(find_tileset(mRegistry, kEmptyTile), kInvalidEntity);
  EXPECT_EQ(find_tileset(mRegistry, TileID {1}), kInvalidEntity);

  const auto ts1_entity = make_dummy_tileset_with_100_tiles();
  const auto ts2_entity = make_dummy_tileset_with_100_tiles();
  const auto ts3_entity = make_dummy_tileset_with_100_tiles();

  // [1, 101)
  ASSERT_TRUE(init_tileset_instance(mRegistry, ts1_entity, TileID {1}).has_value());

  // [101, 201)
  ASSERT_TRUE(init_tileset_instance(mRegistry, ts2_entity, TileID {101}).has_value());

  // [201, 301)
  ASSERT_TRUE(init_tileset_instance(mRegistry, ts3_entity, TileID {201}).has_value());

  EXPECT_EQ(find_tileset(mRegistry, TileID {001}), ts1_entity);
  EXPECT_EQ(find_tileset(mRegistry, TileID {100}), ts1_entity);
  EXPECT_EQ(find_tileset(mRegistry, TileID {101}), ts2_entity);
  EXPECT_EQ(find_tileset(mRegistry, TileID {200}), ts2_entity);
  EXPECT_EQ(find_tileset(mRegistry, TileID {201}), ts3_entity);
  EXPECT_EQ(find_tileset(mRegistry, TileID {300}), ts3_entity);

  EXPECT_EQ(find_tileset(mRegistry, kEmptyTile), kInvalidEntity);
  EXPECT_EQ(find_tileset(mRegistry, TileID {301}), kInvalidEntity);
}

// tactile::core::get_tile_index
TEST_F(TilesetTest, GetTileIndex)
{
  EXPECT_EQ(get_tile_index(mRegistry, kEmptyTile), std::nullopt);
  EXPECT_EQ(get_tile_index(mRegistry, TileID {1}), std::nullopt);

  const auto ts1_entity = make_dummy_tileset_with_100_tiles();
  const auto ts2_entity = make_dummy_tileset_with_100_tiles();

  // [10, 110)
  ASSERT_TRUE(init_tileset_instance(mRegistry, ts1_entity, TileID {10}).has_value());

  // [110, 210)
  ASSERT_TRUE(init_tileset_instance(mRegistry, ts2_entity, TileID {110}).has_value());

  EXPECT_EQ(get_tile_index(mRegistry, TileID {10}), 0);
  EXPECT_EQ(get_tile_index(mRegistry, TileID {53}), 43);
  EXPECT_EQ(get_tile_index(mRegistry, TileID {109}), 99);

  EXPECT_EQ(get_tile_index(mRegistry, TileID {110}), 0);
  EXPECT_EQ(get_tile_index(mRegistry, TileID {137}), 27);
  EXPECT_EQ(get_tile_index(mRegistry, TileID {209}), 99);

  EXPECT_EQ(get_tile_index(mRegistry, TileID {9}), std::nullopt);
  EXPECT_EQ(get_tile_index(mRegistry, TileID {210}), std::nullopt);
}

// tactile::core::is_tile_range_available
TEST_F(TilesetTest, IsTileRangeAvailable)
{
  EXPECT_FALSE(is_tile_range_available(mRegistry, {TileID {-1}, 10}));
  EXPECT_FALSE(is_tile_range_available(mRegistry, {TileID {0}, 100}));
  EXPECT_TRUE(is_tile_range_available(mRegistry, {TileID {1}, 100}));

  // [5, 105)
  const auto ts_entity = make_dummy_tileset_with_100_tiles();
  ASSERT_TRUE(init_tileset_instance(mRegistry, ts_entity, TileID {5}).has_value());

  // Identical range
  EXPECT_FALSE(is_tile_range_available(mRegistry, {TileID {5}, 100}));

  // At limit
  EXPECT_TRUE(is_tile_range_available(mRegistry, {TileID {4}, 1}));
  EXPECT_TRUE(is_tile_range_available(mRegistry, {TileID {105}, 1}));

  // Beyond occupied range
  EXPECT_TRUE(is_tile_range_available(mRegistry, {TileID {1}, 3}));
  EXPECT_TRUE(is_tile_range_available(mRegistry, {TileID {200}, 25}));

  // 1 tile overlap
  EXPECT_FALSE(is_tile_range_available(mRegistry, {TileID {1}, 5}));
  EXPECT_FALSE(is_tile_range_available(mRegistry, {TileID {-1}, 7}));

  // Large overlap
  EXPECT_FALSE(is_tile_range_available(mRegistry, {TileID {1}, 20}));
  EXPECT_FALSE(is_tile_range_available(mRegistry, {TileID {40}, 100}));
}

// tactile::core::has_tile
TEST_F(TilesetTest, HasTile)
{
  // [82, 182)
  const TileRange range {TileID {82}, 100};

  EXPECT_FALSE(has_tile(range, kEmptyTile));
  EXPECT_FALSE(has_tile(range, TileID {81}));
  EXPECT_TRUE(has_tile(range, TileID {82}));
  EXPECT_TRUE(has_tile(range, TileID {123}));
  EXPECT_TRUE(has_tile(range, TileID {181}));
  EXPECT_FALSE(has_tile(range, TileID {182}));
}

}  // namespace
}  // namespace tactile::core
