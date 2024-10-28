// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/map_view_impl.hpp"

#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/layer/object_layer.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/map/map_spec.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class MapViewImplTest : public testing::Test
{
 public:
  MapViewImplTest()
    : mMapSpec {kOrthogonalMapSpec},
      mDocument {MapDocument::make(mMapSpec).value()}
  {}

 protected:
  MapSpec mMapSpec;
  MapDocument mDocument;
};

// tactile::core::MapViewImpl::get_tile_size
// tactile::core::MapViewImpl::get_extent
// tactile::core::MapViewImpl::get_next_layer_id
// tactile::core::MapViewImpl::get_next_object_id
// tactile::core::MapViewImpl::layer_count
// tactile::core::MapViewImpl::tileset_count
TEST_F(MapViewImplTest, Getters)
{
  auto& registry = mDocument.get_registry();
  const auto map_id = registry.get<CDocumentInfo>().root;

  auto& map = registry.get<CMap>(map_id);
  map.attached_tilesets.push_back(registry.make_entity());

  auto& id_cache = registry.get<CMapIdCache>(map_id);
  id_cache.next_layer_id = LayerID {7};
  id_cache.next_object_id = ObjectID {42};

  auto& root_layer = registry.get<CGroupLayer>(map.root_layer);
  root_layer.layers.push_back(make_object_layer(registry));
  root_layer.layers.push_back(make_object_layer(registry));
  root_layer.layers.push_back(make_object_layer(registry));

  const MapViewImpl map_view {&mDocument};

  EXPECT_EQ(map_view.get_tile_size(), map.tile_size);
  EXPECT_EQ(map_view.get_extent(), map.extent);
  EXPECT_EQ(map_view.get_next_layer_id(), id_cache.next_layer_id);
  EXPECT_EQ(map_view.get_next_object_id(), id_cache.next_object_id);
  EXPECT_EQ(map_view.layer_count(), count_layers(registry, map.root_layer));
  EXPECT_EQ(map_view.tileset_count(), map.attached_tilesets.size());
  // TODO component_count
}

}  // namespace
}  // namespace tactile::core
