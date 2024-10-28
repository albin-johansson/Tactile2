// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/tile/tile.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/layer/object.hpp"
#include "tactile/core/meta/meta.hpp"
#include "tactile/core/test/ir_comparison.hpp"
#include "tactile/core/tile/animation.hpp"
#include "tactile/core/tile/animation_types.hpp"
#include "tactile/core/tile/tile_types.hpp"
#include "tactile/test_util/ir_presets.hpp"

namespace tactile::core {
namespace {

class TileTest : public testing::Test
{
 protected:
  Registry mRegistry {};
};

// tactile::core::make_tile [Registry&, TileIndex]
// tactile::core::is_tile
TEST_F(TileTest, MakeTile)
{
  const auto tile_entity = make_tile(mRegistry, TileIndex {42});

  EXPECT_TRUE(is_tile(mRegistry, tile_entity));
  ASSERT_TRUE(mRegistry.has<CMeta>(tile_entity));
  ASSERT_TRUE(mRegistry.has<CTile>(tile_entity));

  const auto& meta = mRegistry.get<CMeta>(tile_entity);
  const auto& tile = mRegistry.get<CTile>(tile_entity);

  EXPECT_EQ(meta.name, "");
  EXPECT_EQ(meta.properties.size(), 0);
  EXPECT_EQ(meta.components.size(), 0);

  EXPECT_EQ(tile.index, TileIndex {42});
  EXPECT_EQ(tile.objects.size(), 0);
}

// tactile::core::make_tile [Registry&, const ir::Tile&]
TEST_F(TileTest, MakeTileFromIR)
{
  ObjectID next_object_id {30};
  const auto ir_tile = test::make_complex_ir_tile(TileIndex {74}, next_object_id);

  const auto tile_id = make_tile(mRegistry, ir_tile);
  ASSERT_TRUE(tile_id.has_value());
  ASSERT_TRUE(is_tile(mRegistry, *tile_id));

  compare_tile(mRegistry, *tile_id, ir_tile);
}

// tactile::core::destroy_tile
TEST_F(TileTest, DestroyTile)
{
  const auto tile_entity = make_tile(mRegistry, TileIndex {10});
  ASSERT_TRUE(mRegistry.is_valid(tile_entity));

  {
    auto& tile = mRegistry.get<CTile>(tile_entity);
    tile.objects.push_back(make_object(mRegistry, ObjectID {1}, ObjectType::kRect));
    tile.objects.push_back(make_object(mRegistry, ObjectID {2}, ObjectType::kEllipse));
    tile.objects.push_back(make_object(mRegistry, ObjectID {3}, ObjectType::kPoint));
  }

  //   Tiles: 1 * (1 CMeta + 1 CTile)
  // Objects: 3 * (1 CMeta + 1 CObject)
  EXPECT_EQ(mRegistry.count(), 8);
  EXPECT_EQ(mRegistry.count<CTile>(), 1);
  EXPECT_EQ(mRegistry.count<CMeta>(), 4);
  EXPECT_EQ(mRegistry.count<CObject>(), 3);

  destroy_tile(mRegistry, tile_entity);

  EXPECT_FALSE(mRegistry.is_valid(tile_entity));
  EXPECT_FALSE(is_tile(mRegistry, tile_entity));
  EXPECT_EQ(mRegistry.count(), 0);
  EXPECT_EQ(mRegistry.count<CTile>(), 0);
  EXPECT_EQ(mRegistry.count<CObject>(), 0);
}

// tactile::core::copy_tile
TEST_F(TileTest, CopyTile)
{
  const auto e1 = make_tile(mRegistry, TileIndex {35});

  auto& meta1 = mRegistry.get<CMeta>(e1);
  meta1.name = "abcdef";
  meta1.properties["x"] = Attribute {"y"};
  meta1.components[UUID::generate()];

  auto& tile1 = mRegistry.get<CTile>(e1);
  tile1.objects.push_back(make_object(mRegistry, ObjectID {1}, ObjectType::kPoint));
  tile1.objects.push_back(make_object(mRegistry, ObjectID {2}, ObjectType::kRect));
  tile1.objects.push_back(make_object(mRegistry, ObjectID {3}, ObjectType::kEllipse));

  const auto e2 = copy_tile(mRegistry, e1);
  EXPECT_TRUE(is_tile(mRegistry, e2));

  const auto& meta2 = mRegistry.get<CMeta>(e2);
  EXPECT_EQ(meta2.name, meta1.name);
  EXPECT_EQ(meta2.properties, meta1.properties);
  EXPECT_EQ(meta2.components, meta1.components);

  const auto& tile2 = mRegistry.get<CTile>(e2);
  EXPECT_EQ(tile2.index, tile1.index);
  EXPECT_EQ(tile2.objects.size(), tile1.objects.size());
  EXPECT_NE(tile2.objects, tile1.objects);
}

// tactile::core::is_tile_plain
TEST_F(TileTest, IsTilePlain)
{
  const auto id1 = make_tile(mRegistry, TileIndex {1});
  const auto id2 = make_tile(mRegistry, TileIndex {2});
  const auto id3 = make_tile(mRegistry, TileIndex {3});
  const auto id4 = make_tile(mRegistry, TileIndex {4});
  const auto id5 = make_tile(mRegistry, TileIndex {5});

  {
    const AnimationFrame frame {TileIndex {10}, std::chrono::milliseconds {50}};
    ASSERT_TRUE(add_animation_frame(mRegistry, id2, 0, frame));
  }

  {
    auto& tile3 = mRegistry.get<CTile>(id3);
    tile3.objects.push_back(make_object(mRegistry, ObjectID {1}, ObjectType::kPoint));
  }

  {
    auto& meta4 = mRegistry.get<CMeta>(id4);
    meta4.properties["foo"] = Attribute {"bar"};
  }

  {
    auto& meta5 = mRegistry.get<CMeta>(id5);
    meta5.components[UUID::generate()];
  }

  EXPECT_TRUE(is_tile_plain(mRegistry, id1));
  EXPECT_FALSE(is_tile_plain(mRegistry, id2));  // Animated
  EXPECT_FALSE(is_tile_plain(mRegistry, id3));  // Has object
  EXPECT_FALSE(is_tile_plain(mRegistry, id4));  // Has property
  EXPECT_FALSE(is_tile_plain(mRegistry, id5));  // Has component
}

}  // namespace
}  // namespace tactile::core
