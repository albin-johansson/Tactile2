// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/document/map_view.hpp"
#include "tactile/base/prelude.hpp"

namespace tactile {

struct CMap;
struct CMapIdCache;
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
  auto layer_count() const -> usize override;

  [[nodiscard]]
  auto tileset_count() const -> usize override;

  [[nodiscard]]
  auto component_count() const -> usize override;

 private:
  const MapDocument* mDocument;

  [[nodiscard]]
  auto _get_map() const -> const CMap&;

  [[nodiscard]]
  auto _get_id_cache() const -> const CMapIdCache&;
};

}  // namespace tactile
