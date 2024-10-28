// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/object_view_impl.hpp"

#include <gtest/gtest.h>

#include "tactile/core/document/layer_view_impl.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/document/tile_view_impl.hpp"
#include "tactile/core/document/tileset_view_impl.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/layer/object.hpp"
#include "tactile/core/layer/object_layer.hpp"
#include "tactile/core/map/map_spec.hpp"
#include "tactile/core/tile/tile_types.hpp"
#include "tactile/core/tile/tileset.hpp"
#include "tactile/core/tile/tileset_types.hpp"
#include "test/document_testing.hpp"

namespace tactile::core {
namespace {

class ObjectViewImplTest : public testing::Test
{
 public:
  ObjectViewImplTest()
    : mMapSpec {kOrthogonalMapSpec},
      mDocument {MapDocument::make(mMapSpec).value()}
  {}

 protected:
  MapSpec mMapSpec;
  MapDocument mDocument;
};

// tactile::core::ObjectViewImpl::ObjectViewImpl
TEST_F(ObjectViewImplTest, Constructors)
{
  const ILayerView* layer_view = nullptr;
  const ITileView* tile_view = nullptr;

  EXPECT_ANY_THROW(ObjectViewImpl(nullptr, layer_view, kInvalidEntity));
  EXPECT_ANY_THROW(ObjectViewImpl(nullptr, tile_view, kInvalidEntity));

  EXPECT_ANY_THROW(ObjectViewImpl(&mDocument, layer_view, kInvalidEntity));
  EXPECT_ANY_THROW(ObjectViewImpl(&mDocument, tile_view, kInvalidEntity));
}

// tactile::core::ObjectViewImpl::get_parent_layer
// tactile::core::ObjectViewImpl::get_parent_tile
// tactile::core::ObjectViewImpl::get_type
// tactile::core::ObjectViewImpl::get_id
// tactile::core::ObjectViewImpl::get_position
// tactile::core::ObjectViewImpl::get_size
// tactile::core::ObjectViewImpl::get_tag
// tactile::core::ObjectViewImpl::is_visible
TEST_F(ObjectViewImplTest, ObjectInObjectLayer)
{
  auto& registry = mDocument.get_registry();

  const auto layer_id = make_object_layer(registry);
  const auto object_id = make_object(registry, ObjectID {1}, ObjectType::kRect);

  auto& object_layer = registry.get<CObjectLayer>(layer_id);
  object_layer.objects.push_back(object_id);

  auto& object = registry.get<CObject>(object_id);
  object.position = Float2 {12, 34};
  object.size = Float2 {56, 78};
  object.tag = "tag";

  const LayerViewImpl layer_view {&mDocument, nullptr, layer_id};
  const ObjectViewImpl object_view {&mDocument, &layer_view, object_id};

  EXPECT_EQ(object_view.get_parent_layer(), &layer_view);
  EXPECT_EQ(object_view.get_parent_tile(), nullptr);
  EXPECT_EQ(object_view.get_type(), object.type);
  EXPECT_EQ(object_view.get_id(), object.id);
  EXPECT_EQ(object_view.get_position(), object.position);
  EXPECT_EQ(object_view.get_size(), object.size);
  EXPECT_EQ(object_view.get_tag(), object.tag);
  EXPECT_EQ(object_view.is_visible(), object.is_visible);
}

// tactile::core::ObjectViewImpl::get_parent_layer
// tactile::core::ObjectViewImpl::get_parent_tile
// tactile::core::ObjectViewImpl::get_type
// tactile::core::ObjectViewImpl::get_id
// tactile::core::ObjectViewImpl::get_position
// tactile::core::ObjectViewImpl::get_size
// tactile::core::ObjectViewImpl::get_tag
// tactile::core::ObjectViewImpl::is_visible
TEST_F(ObjectViewImplTest, ObjectInTile)
{
  auto& registry = mDocument.get_registry();

  const auto tileset_id = make_tileset(registry, kDummyTilesetSpec);
  const auto& tileset = registry.get<CTileset>(tileset_id);

  const auto tile_id = tileset.tiles.front();
  const auto object_id = make_object(registry, ObjectID {1}, ObjectType::kRect);

  auto& tile = registry.get<CTile>(tile_id);
  tile.objects.push_back(object_id);

  auto& object = registry.get<CObject>(object_id);
  object.position = Float2 {-23, 15};
  object.size = Float2 {10, 12};
  object.tag = "foo";

  const TilesetViewImpl tileset_view {&mDocument, tileset_id};
  const TileViewImpl tile_view {&mDocument, &tileset_view, tile_id};
  const ObjectViewImpl object_view {&mDocument, &tile_view, object_id};

  EXPECT_EQ(object_view.get_parent_layer(), nullptr);
  EXPECT_EQ(object_view.get_parent_tile(), &tile_view);
  EXPECT_EQ(object_view.get_type(), object.type);
  EXPECT_EQ(object_view.get_id(), object.id);
  EXPECT_EQ(object_view.get_position(), object.position);
  EXPECT_EQ(object_view.get_size(), object.size);
  EXPECT_EQ(object_view.get_tag(), object.tag);
  EXPECT_EQ(object_view.is_visible(), object.is_visible);
}

}  // namespace
}  // namespace tactile::core
