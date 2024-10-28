// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/layer_view_impl.hpp"

#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer.hpp"
#include "tactile/core/layer/object_layer.hpp"
#include "tactile/core/layer/tile_layer.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/map/map_spec.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class LayerViewImplTest : public testing::Test
{
 public:
  LayerViewImplTest()
    : mMapSpec {kOrthogonalMapSpec},
      mDocument {MapDocument::make(mMapSpec).value()}
  {}

  void SetUp() override
  {
    auto& registry = mDocument.get_registry();
    const auto map_id = registry.get<CDocumentInfo>().root;

    auto& map = registry.get<CMap>(map_id);
    auto& root_layer = registry.get<CGroupLayer>(map.root_layer);

    mObjectLayerId = make_object_layer(registry);
    mTileLayerId = make_tile_layer(registry, mMapSpec.extent);

    root_layer.layers.push_back(mObjectLayerId);
    root_layer.layers.push_back(mTileLayerId);
  }

 protected:
  MapSpec mMapSpec;
  MapDocument mDocument;
  EntityID mTileLayerId {kInvalidEntity};
  EntityID mObjectLayerId {kInvalidEntity};
};

// tactile::core::LayerViewImpl::LayerViewImpl
TEST_F(LayerViewImplTest, Constructor)
{
  EXPECT_ANY_THROW(LayerViewImpl(nullptr, nullptr, kInvalidEntity));
}

// tactile::core::LayerViewImpl::get_parent_layer
// tactile::core::LayerViewImpl::get_type
// tactile::core::LayerViewImpl::get_opacity
// tactile::core::LayerViewImpl::is_visible
// tactile::core::LayerViewImpl::get_global_index
// tactile::core::LayerViewImpl::get_tile
// tactile::core::LayerViewImpl::get_extent
TEST_F(LayerViewImplTest, Getters)
{
  auto& registry = mDocument.get_registry();

  const auto& map = registry.get<CMap>(registry.get<CDocumentInfo>().root);
  const auto layer_id = mTileLayerId;

  auto& layer = registry.get<CLayer>(layer_id);
  layer.persistent_id = 42;
  layer.opacity = 0.5f;
  layer.visible = false;

  const LayerViewImpl layer_view {&mDocument, nullptr, layer_id};

  EXPECT_EQ(layer_view.get_parent_layer(), nullptr);
  EXPECT_EQ(layer_view.get_type(), LayerType::kTileLayer);
  EXPECT_EQ(layer_view.get_opacity(), layer.opacity);
  EXPECT_EQ(layer_view.is_visible(), layer.visible);
  EXPECT_EQ(layer_view.get_global_index(),
            get_global_layer_index(registry, map.root_layer, layer_id));

  EXPECT_EQ(layer_view.get_extent(), mMapSpec.extent);
  EXPECT_EQ(layer_view.get_tile(Index2D {0, 0}), kEmptyTile);
}

}  // namespace
}  // namespace tactile::core
