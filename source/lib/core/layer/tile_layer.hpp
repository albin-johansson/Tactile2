// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "core/layer/abstract_layer.hpp"
#include "core/tile/tile_extent.hpp"
#include "core/tile/tile_matrix.hpp"
#include "core/tile/tile_pos.hpp"
#include "tactile/base/container/expected.hpp"
#include "tactile/base/container/maybe.hpp"
#include "tactile/base/container/vector.hpp"
#include "tactile/base/int.hpp"

namespace tactile {

/// A layer variant consisting of a matrix of tile identifiers.
class TileLayer final : public AbstractLayer {
 public:
  TileLayer();

  explicit TileLayer(TileExtent extent);

  ~TileLayer() noexcept override = default;

  void accept(ContextVisitor& visitor) const override;
  void accept(LayerVisitor& visitor) override;
  void accept(ConstLayerVisitor& visitor) const override;

  /// Runs a flood-fill algorithm in the layer, used by the bucket tool.
  ///
  /// <p>
  /// This will replace all adjacent tiles of the type at the origin position with the
  /// specified tile replacement.
  ///
  /// \param origin the initial position of the flood-fill.
  /// \param replacement the tile ID that will be used when
  /// \param[out] affected pointer to which all modified positions are written to.
  void flood(const TilePos& origin,
             TileID replacement,
             Vector<TilePos>* affected = nullptr);

  /// Appends a single row of empty tiles to the bottom of the layer.
  void add_row();

  /// Appends a single column of empty tiles to the end of the layer.
  void add_column();

  /// Removes a single row from the layer.
  /// There must be more than one row in the layer when this function is called.
  auto remove_row() -> Result<void>;

  /// Removes a single column from the layer.
  /// There must be more than one column in the layer when this function is called.
  auto remove_column() -> Result<void>;

  /// Changes the size of the layer.
  /// Note, layers must have at least 1 row and 1 column.
  void resize(TileExtent extent);

  /// Sets the tile identifier at the specified position.
  auto set_tile(const TilePos& pos, TileID id) -> Result<void>;

  /// Returns the tile identifier at a specific position.
  [[nodiscard]] auto tile_at(const TilePos& pos) const -> Maybe<TileID>;

  /// Indicates whether a tile position refers to a valid tile in the layer.
  [[nodiscard]] auto is_valid(const TilePos& pos) const -> bool;

  /// Returns the number of tile rows in the layer.
  [[nodiscard]] auto row_count() const -> usize;

  /// Returns the number of tile columns in the layer.
  [[nodiscard]] auto column_count() const -> usize;

  /// Returns the associated tile matrix.
  [[nodiscard]] auto get_tiles() const -> const TileMatrix&;

  [[nodiscard]] auto clone() const -> Shared<Layer> override;

  [[nodiscard]] auto get_type() const -> LayerType override
  {
    return LayerType::TileLayer;
  }

 private:
  TileMatrix mTiles;
};

}  // namespace tactile
