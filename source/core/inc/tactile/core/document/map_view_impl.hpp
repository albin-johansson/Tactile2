// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/document/map_view.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/core/document/meta_view_impl.hpp"

namespace tactile {

struct CMap;
struct CMapIdCache;
struct CTileFormat;
class MapDocument;

/**
 * A map view implementation.
 */
class MapViewImpl final : public IMapView
{
 public:
  /**
   * Creates a view of a map.
   *
   * \param document The associated map document.
   */
  MapViewImpl(const MapDocument* document);

  void accept(IDocumentVisitor& visitor) const override;

  [[nodiscard]]
  auto get_tile_size() const -> Int2 override;

  [[nodiscard]]
  auto get_extent() const -> MatrixExtent override;

  [[nodiscard]]
  auto get_next_layer_id() const -> LayerID override;

  [[nodiscard]]
  auto get_next_object_id() const -> ObjectID override;

  [[nodiscard]]
  auto get_tile_encoding() const -> TileEncoding override;

  [[nodiscard]]
  auto get_tile_compression() const -> Optional<CompressionFormat> override;

  [[nodiscard]]
  auto get_compression_level() const -> Optional<int> override;

  [[nodiscard]]
  auto layer_count() const -> usize override;

  [[nodiscard]]
  auto tileset_count() const -> usize override;

  [[nodiscard]]
  auto component_count() const -> usize override;

  [[nodiscard]]
  auto get_meta() const -> const IMetaView& override;

 private:
  const MapDocument* mDocument;
  MetaViewImpl mMeta;

  [[nodiscard]]
  auto _get_map() const -> const CMap&;

  [[nodiscard]]
  auto _get_id_cache() const -> const CMapIdCache&;

  [[nodiscard]]
  auto _get_tile_format() const -> const CTileFormat&;
};

}  // namespace tactile