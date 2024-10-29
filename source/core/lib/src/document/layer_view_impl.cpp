// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/layer_view_impl.hpp"

#include <stdexcept>  // invalid_argument

#include "tactile/base/debug/validation.hpp"
#include "tactile/base/document/document_visitor.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/document/object_view_impl.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer.hpp"
#include "tactile/core/layer/object_layer.hpp"
#include "tactile/core/layer/tile_layer.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/meta/meta.hpp"
#include "tactile/core/tile/animation_types.hpp"
#include "tactile/core/tile/tileset.hpp"
#include "tactile/core/tile/tileset_types.hpp"

namespace tactile::core {

LayerViewImpl::LayerViewImpl(const MapDocument* document,
                             const ILayerView* parent_layer,
                             const EntityID layer_id)
  : mDocument {require_not_null(document, "null document")},
    mParentLayer {parent_layer},
    mLayerId {layer_id},
    mMeta {document, mLayerId}
{}

auto LayerViewImpl::accept(IDocumentVisitor& visitor) const -> std::expected<void, ErrorCode>
{
  const auto& registry = mDocument->get_registry();

  if (const auto layer_result = visitor.visit(*this); !layer_result.has_value()) {
    return std::unexpected {layer_result.error()};
  }

  if (is_group_layer(registry, mLayerId)) {
    const auto& group_layer = registry.get<CGroupLayer>(mLayerId);

    for (const auto sublayer_id : group_layer.layers) {
      const LayerViewImpl sublayer_view {mDocument, this, sublayer_id};
      if (const auto sublayer_result = sublayer_view.accept(visitor);
          !sublayer_result.has_value()) {
        return std::unexpected {sublayer_result.error()};
      }
    }
  }
  else if (is_object_layer(registry, mLayerId)) {
    const auto& object_layer = registry.get<CObjectLayer>(mLayerId);

    for (const auto object_id : object_layer.objects) {
      const ObjectViewImpl object_view {mDocument, this, object_id};
      if (const auto object_result = object_view.accept(visitor); !object_result.has_value()) {
        return std::unexpected {object_result.error()};
      }
    }
  }

  return {};
}

void LayerViewImpl::write_tile_bytes(ByteStream& byte_stream) const
{
  const auto& registry = mDocument->get_registry();

  if (is_tile_layer(registry, mLayerId)) {
    byte_stream = serialize_tile_layer(registry, mLayerId);
  }
}

auto LayerViewImpl::get_parent_layer() const -> const ILayerView*
{
  return mParentLayer;
}

auto LayerViewImpl::get_id() const -> LayerID
{
  const auto& registry = mDocument->get_registry();
  const auto& layer = registry.get<CLayer>(mLayerId);

  return layer.persistent_id.value();
}

auto LayerViewImpl::get_type() const -> LayerType
{
  const auto& registry = mDocument->get_registry();

  if (is_tile_layer(registry, mLayerId)) {
    return LayerType::kTileLayer;
  }

  if (is_object_layer(registry, mLayerId)) {
    return LayerType::kObjectLayer;
  }

  if (is_group_layer(registry, mLayerId)) {
    return LayerType::kGroupLayer;
  }

  throw std::invalid_argument {"unexpected layer type"};
}

auto LayerViewImpl::get_opacity() const -> float
{
  const auto& registry = mDocument->get_registry();
  const auto& layer = registry.get<CLayer>(mLayerId);

  return layer.opacity;
}

auto LayerViewImpl::is_visible() const -> bool
{
  const auto& registry = mDocument->get_registry();
  const auto& layer = registry.get<CLayer>(mLayerId);

  return layer.visible;
}

auto LayerViewImpl::get_global_index() const -> std::size_t
{
  const auto& registry = mDocument->get_registry();

  const auto& document_info = registry.get<CDocumentInfo>();
  const auto& map = registry.get<CMap>(document_info.root);

  return get_global_layer_index(registry, map.root_layer, mLayerId).value();
}

auto LayerViewImpl::layer_count() const -> std::size_t
{
  const auto& registry = mDocument->get_registry();

  if (const auto* group = registry.find<CGroupLayer>(mLayerId)) {
    return group->layers.size();
  }

  return 0;
}

auto LayerViewImpl::object_count() const -> std::size_t
{
  const auto& registry = mDocument->get_registry();

  if (const auto* object_layer = registry.find<CObjectLayer>(mLayerId)) {
    return object_layer->objects.size();
  }

  return 0;
}

auto LayerViewImpl::get_tile(const Index2D& index) const -> std::optional<TileID>
{
  const auto& registry = mDocument->get_registry();

  if (!is_tile_layer(registry, mLayerId)) {
    return std::nullopt;
  }

  return get_layer_tile(registry, mLayerId, index);
}

auto LayerViewImpl::get_tile_position_in_tileset(const TileID tile_id) const
    -> std::optional<Index2D>
{
  const auto& registry = mDocument->get_registry();

  const auto tile_index = get_tile_index(registry, tile_id);
  if (!tile_index.has_value()) {
    return std::nullopt;
  }

  const auto tileset_id = find_tileset(registry, tile_id);
  if (tileset_id == kInvalidEntity) {
    return std::nullopt;
  }

  const auto& tileset = registry.get<CTileset>(tileset_id);
  return Index2D::from_1d(static_cast<Index2D::value_type>(*tile_index), tileset.extent.cols);
}

auto LayerViewImpl::is_tile_animated(const Index2D& position) const -> bool
{
  const auto& registry = mDocument->get_registry();

  const auto tile_id = get_layer_tile(registry, mLayerId, position).value();
  if (tile_id == kEmptyTile) {
    return false;
  }

  const auto tileset_id = find_tileset(registry, tile_id);
  if (tileset_id == kInvalidEntity) {
    return false;
  }

  const auto& tileset = registry.get<CTileset>(tileset_id);
  const auto& tileset_instance = registry.get<CTilesetInstance>(tileset_id);

  const auto tile_index = tile_id - tileset_instance.tile_range.first_id;
  const auto tile_entity = tileset.tiles.at(static_cast<std::size_t>(tile_index));

  return registry.has<CAnimation>(tile_entity);
}

auto LayerViewImpl::get_tile_encoding() const -> TileEncoding
{
  return _get_tile_format().encoding;
}

auto LayerViewImpl::get_tile_compression() const -> std::optional<CompressionFormatId>
{
  return _get_tile_format().compression;
}

auto LayerViewImpl::get_compression_level() const -> std::optional<int>
{
  return _get_tile_format().comp_level;
}

auto LayerViewImpl::get_extent() const -> std::optional<Extent2D>
{
  const auto& registry = mDocument->get_registry();
  const auto* tile_layer = registry.find<CTileLayer>(mLayerId);

  if (!tile_layer) {
    return std::nullopt;
  }

  return tile_layer->extent;
}

auto LayerViewImpl::get_meta() const -> const IMetaView&
{
  return mMeta;
}

auto LayerViewImpl::_get_tile_format() const -> const CTileFormat&
{
  const auto& registry = mDocument->get_registry();
  const auto& document_info = registry.get<CDocumentInfo>();
  return registry.get<CTileFormat>(document_info.root);
}

}  // namespace tactile::core
