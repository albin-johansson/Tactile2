// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/map_view_impl.hpp"

#include "tactile/base/document/document_visitor.hpp"
#include "tactile/core/debug/validation.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/layer_view_impl.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/document/tileset_view_impl.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/map/map.hpp"

namespace tactile {

MapViewImpl::MapViewImpl(const MapDocument* document)
  : mDocument {require_not_null(document, "null document")},
    mMeta {mDocument, mDocument->get_registry().get<CDocumentInfo>().root}
{}

auto MapViewImpl::accept(IDocumentVisitor& visitor) const -> Result<void>
{
  const auto& registry = mDocument->get_registry();

  const auto& map = _get_map();
  const auto& root_layer = registry.get<CGroupLayer>(map.root_layer);

  if (const auto map_result = visitor.visit(*this); !map_result.has_value()) {
    return propagate_unexpected(map_result);
  }

  for (const auto tileset_id : map.attached_tilesets) {
    const TilesetViewImpl tileset_view {mDocument, tileset_id};
    if (const auto tileset_result = tileset_view.accept(visitor);
        !tileset_result.has_value()) {
      return propagate_unexpected(tileset_result);
    }
  }

  for (const auto layer_id : root_layer.layers) {
    const LayerViewImpl layer_view {mDocument, nullptr, layer_id};
    if (const auto layer_result = layer_view.accept(visitor); !layer_result.has_value()) {
      return propagate_unexpected(layer_result);
    }
  }

  // TODO iterate component definitions

  return kOK;
}

auto MapViewImpl::get_tile_size() const -> Int2
{
  return _get_map().tile_size;
}

auto MapViewImpl::get_extent() const -> MatrixExtent
{
  return _get_map().extent;
}

auto MapViewImpl::get_next_layer_id() const -> LayerID
{
  return _get_id_cache().next_layer_id;
}

auto MapViewImpl::get_next_object_id() const -> ObjectID
{
  return _get_id_cache().next_object_id;
}

auto MapViewImpl::get_tile_encoding() const -> TileEncoding
{
  return _get_tile_format().encoding;
}

auto MapViewImpl::get_tile_compression() const -> Optional<CompressionFormat>
{
  return _get_tile_format().compression;
}

auto MapViewImpl::get_compression_level() const -> Optional<int>
{
  return _get_tile_format().comp_level;
}

auto MapViewImpl::layer_count() const -> usize
{
  const auto& registry = mDocument->get_registry();
  const auto& map = _get_map();

  return count_layers(registry, map.root_layer);
}

auto MapViewImpl::tileset_count() const -> usize
{
  return _get_map().attached_tilesets.size();
}

auto MapViewImpl::component_count() const -> usize
{
  return 0;  // TODO
}

auto MapViewImpl::get_meta() const -> const IMetaView&
{
  return mMeta;
}

auto MapViewImpl::_get_map() const -> const CMap&
{
  const auto& registry = mDocument->get_registry();
  const auto& document_info = registry.get<CDocumentInfo>();
  return registry.get<CMap>(document_info.root);
}

auto MapViewImpl::_get_id_cache() const -> const CMapIdCache&
{
  const auto& registry = mDocument->get_registry();
  const auto& document_info = registry.get<CDocumentInfo>();
  return registry.get<CMapIdCache>(document_info.root);
}

auto MapViewImpl::_get_tile_format() const -> const CTileFormat&
{
  const auto& registry = mDocument->get_registry();
  const auto& document_info = registry.get<CDocumentInfo>();
  return registry.get<CTileFormat>(document_info.root);
}

}  // namespace tactile
