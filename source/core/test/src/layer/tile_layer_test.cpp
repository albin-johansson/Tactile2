// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/layer/tile_layer.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tactile/base/io/tile_io.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {
namespace {

class TileLayerTest : public testing::TestWithParam<bool>
{
 protected:
  Registry mRegistry {};
  bool mTestingDenseLayer {};

  void SetUp() override
  {
    mTestingDenseLayer = GetParam();
  }

  [[nodiscard]]
  auto make_test_layer(const Extent2D& extent = {5, 5})
  {
    const auto layer_id = make_tile_layer(mRegistry, extent);

    if (mTestingDenseLayer) {
      convert_to_dense_tile_layer(mRegistry, layer_id);
    }
    else {
      convert_to_sparse_tile_layer(mRegistry, layer_id);
    }

    return layer_id;
  }

  void check_tile_validity(const EntityID layer_id, const Extent2D& expected_extent)
  {
    const auto& tile_layer = mRegistry.get<CTileLayer>(layer_id);
    EXPECT_EQ(tile_layer.extent, expected_extent);

    for (Extent2D::value_type row = 0; row < expected_extent.rows; ++row) {
      for (Extent2D::value_type col = 0; col < expected_extent.cols; ++col) {
        const Index2D index {.x = col, .y = row};
        EXPECT_TRUE(get_layer_tile(mRegistry, layer_id, index).has_value());
      }
    }
  }

  void test_resize(const EntityID layer_id, const Extent2D& original_extent)
  {
    const auto resize_and_validate = [&](const Extent2D& extent) {
      resize_tile_layer(mRegistry, layer_id, extent);
      check_tile_validity(layer_id, extent);
    };

    resize_and_validate(Extent2D {original_extent.rows + 1, original_extent.cols});
    resize_and_validate(Extent2D {original_extent.rows - 1, original_extent.cols});

    resize_and_validate(Extent2D {original_extent.rows, original_extent.cols + 1});
    resize_and_validate(Extent2D {original_extent.rows, original_extent.cols - 1});

    resize_and_validate(Extent2D {original_extent.rows + 2, original_extent.cols + 3});
    resize_and_validate(Extent2D {original_extent.rows - 2, original_extent.cols - 3});

    resize_and_validate(Extent2D {0, 0});
    resize_and_validate(Extent2D {20, 20});
    resize_and_validate(Extent2D {30, 10});
    resize_and_validate(Extent2D {10, 30});
  }
};

INSTANTIATE_TEST_SUITE_P(DenseAndSparse, TileLayerTest, testing::Bool());

// tactile::core::is_tile_layer
TEST_P(TileLayerTest, IsTileLayer)
{
  EXPECT_FALSE(is_tile_layer(mRegistry, kInvalidEntity));
  EXPECT_FALSE(is_tile_layer(mRegistry, mRegistry.make_entity()));

  const auto layer_id = make_test_layer();
  EXPECT_TRUE(is_tile_layer(mRegistry, layer_id));
}

// tactile::core::make_tile_layer
TEST_P(TileLayerTest, MakeTileLayer)
{
  constexpr Extent2D extent {5, 6};
  const auto layer_id = make_test_layer(extent);

  EXPECT_TRUE(is_layer(mRegistry, layer_id));
  EXPECT_TRUE(is_tile_layer(mRegistry, layer_id));
  EXPECT_TRUE(mRegistry.has<CMeta>(layer_id));
  EXPECT_TRUE(mRegistry.has<CLayer>(layer_id));
  EXPECT_TRUE(mRegistry.has<CTileLayer>(layer_id));

  if (mTestingDenseLayer) {
    EXPECT_TRUE(mRegistry.has<CDenseTileLayer>(layer_id));
  }
  else {
    EXPECT_TRUE(mRegistry.has<CSparseTileLayer>(layer_id));
  }

  const auto& meta = mRegistry.get<CMeta>(layer_id);
  const auto& layer = mRegistry.get<CLayer>(layer_id);

  EXPECT_EQ(meta.name, "");
  EXPECT_EQ(meta.properties.size(), 0);
  EXPECT_EQ(meta.components.size(), 0);

  EXPECT_EQ(layer.persistent_id, std::nullopt);
  EXPECT_EQ(layer.opacity, 1.0f);
  EXPECT_TRUE(layer.visible);

  each_layer_tile(mRegistry, layer_id, [](const Index2D&, const TileID tile_id) {
    EXPECT_EQ(tile_id, kEmptyTile);
  });
}

// tactile::core::destroy_tile_layer
TEST_P(TileLayerTest, DestroyTileLayer)
{
  const auto layer_id = make_test_layer();

  EXPECT_TRUE(mRegistry.is_valid(layer_id));
  EXPECT_EQ(mRegistry.count<CMeta>(), 1);
  EXPECT_EQ(mRegistry.count<CLayer>(), 1);
  EXPECT_EQ(mRegistry.count<CTileLayer>(), 1);
  EXPECT_EQ(mRegistry.count<CDenseTileLayer>(), mTestingDenseLayer ? 1 : 0);
  EXPECT_EQ(mRegistry.count<CSparseTileLayer>(), mTestingDenseLayer ? 0 : 1);
  EXPECT_EQ(mRegistry.count(), 4);

  destroy_tile_layer(mRegistry, layer_id);

  EXPECT_FALSE(mRegistry.is_valid(layer_id));
  EXPECT_EQ(mRegistry.count<CMeta>(), 0);
  EXPECT_EQ(mRegistry.count<CLayer>(), 0);
  EXPECT_EQ(mRegistry.count<CTileLayer>(), 0);
  EXPECT_EQ(mRegistry.count<CDenseTileLayer>(), 0);
  EXPECT_EQ(mRegistry.count<CSparseTileLayer>(), 0);
  EXPECT_EQ(mRegistry.count(), 0);
}

// tactile::core::convert_to_dense_tile_layer
// tactile::core::convert_to_sparse_tile_layer
TEST_P(TileLayerTest, DenseSparseConversion)
{
  const auto layer_id = make_test_layer(Extent2D {6, 4});

  convert_to_dense_tile_layer(mRegistry, layer_id);

  ASSERT_TRUE(mRegistry.has<CDenseTileLayer>(layer_id));
  ASSERT_FALSE(mRegistry.has<CSparseTileLayer>(layer_id));

  {
    auto& dense = mRegistry.get<CDenseTileLayer>(layer_id);
    dense.tiles.at(0).at(0) = TileID {42};
    dense.tiles.at(0).at(1) = TileID {73};
    dense.tiles.at(3).at(2) = TileID {99};
    dense.tiles.at(5).at(3) = TileID {36};
  }

  convert_to_sparse_tile_layer(mRegistry, layer_id);

  ASSERT_TRUE(mRegistry.has<CSparseTileLayer>(layer_id));
  ASSERT_FALSE(mRegistry.has<CDenseTileLayer>(layer_id));

  {
    const auto& sparse = mRegistry.get<CSparseTileLayer>(layer_id);
    EXPECT_EQ(sparse.tiles.size(), 4);
    EXPECT_EQ(sparse.tiles.at(Index2D {0, 0}), TileID {42});
    EXPECT_EQ(sparse.tiles.at(Index2D {1, 0}), TileID {73});
    EXPECT_EQ(sparse.tiles.at(Index2D {2, 3}), TileID {99});
    EXPECT_EQ(sparse.tiles.at(Index2D {3, 5}), TileID {36});
  }

  convert_to_dense_tile_layer(mRegistry, layer_id);

  ASSERT_TRUE(mRegistry.has<CDenseTileLayer>(layer_id));
  ASSERT_FALSE(mRegistry.has<CSparseTileLayer>(layer_id));

  {
    const auto& dense = mRegistry.get<CDenseTileLayer>(layer_id);
    EXPECT_EQ(dense.tiles.at(0).at(0), TileID {42});
    EXPECT_EQ(dense.tiles.at(0).at(1), TileID {73});
    EXPECT_EQ(dense.tiles.at(3).at(2), TileID {99});
    EXPECT_EQ(dense.tiles.at(5).at(3), TileID {36});
  }
}

// tactile::core::resize_tile_layer
TEST_P(TileLayerTest, ResizeTileLayer)
{
  constexpr Extent2D extent {5, 5};
  const auto layer_id = make_test_layer(extent);
  test_resize(layer_id, extent);
}

// tactile::core::serialize_tile_layer
TEST_P(TileLayerTest, SerializeTileLayer)
{
  constexpr Extent2D extent {3, 3};
  const auto layer_id = make_test_layer(extent);

  convert_to_dense_tile_layer(mRegistry, layer_id);

  {
    auto& dense = mRegistry.get<CDenseTileLayer>(layer_id);
    dense.tiles[0][0] = TileID {11};
    dense.tiles[0][1] = TileID {12};
    dense.tiles[0][2] = TileID {13};
    dense.tiles[1][0] = TileID {21};
    dense.tiles[1][1] = TileID {22};
    dense.tiles[1][2] = TileID {23};
    dense.tiles[2][0] = TileID {31};
    dense.tiles[2][1] = TileID {32};
    dense.tiles[2][2] = TileID {33};
  }

  if (!mTestingDenseLayer) {
    convert_to_sparse_tile_layer(mRegistry, layer_id);
  }

  const auto serialized_tiles = serialize_tile_layer(mRegistry, layer_id);
  const auto deserialized_tiles =
      parse_raw_tile_matrix(serialized_tiles, extent, TileIdFormat::kTactile);

  ASSERT_TRUE(deserialized_tiles.has_value());

  convert_to_dense_tile_layer(mRegistry, layer_id);

  const auto& dense = mRegistry.get<CDenseTileLayer>(layer_id);
  EXPECT_THAT(dense.tiles, testing::ContainerEq(*deserialized_tiles));
}

// tactile::core::set_layer_tile
// tactile::core::get_layer_tile
TEST_P(TileLayerTest, SetLayerTile)
{
  constexpr Extent2D extent {10, 10};
  const auto layer_id = make_test_layer(extent);

  EXPECT_NO_THROW(set_layer_tile(mRegistry, layer_id, Index2D {0, extent.rows}, TileID {99}));
  EXPECT_NO_THROW(set_layer_tile(mRegistry, layer_id, Index2D {extent.cols, 0}, TileID {99}));

  auto set_and_verify = [this, layer_id](const Index2D& index, const TileID tile_id) {
    set_layer_tile(mRegistry, layer_id, index, tile_id);
    EXPECT_EQ(get_layer_tile(mRegistry, layer_id, index), tile_id);
  };

  set_and_verify(Index2D {0, 0}, TileID {1});
  set_and_verify(Index2D {extent.cols - 1, 0}, TileID {2});
  set_and_verify(Index2D {0, extent.rows - 1}, TileID {3});
  set_and_verify(Index2D {extent.cols - 1, extent.rows - 1}, TileID {4});

  set_and_verify(Index2D {6, 3}, TileID {184});
  set_and_verify(Index2D {4, 9}, TileID {865});
}

}  // namespace
}  // namespace tactile::core
