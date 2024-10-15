// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <concepts>    // invocable
#include <functional>  // less
#include <map>         // map
#include <optional>    // optional

#include "tactile/base/id.hpp"
#include "tactile/base/io/byte_stream.hpp"
#include "tactile/base/numeric/extent_2d.hpp"
#include "tactile/base/numeric/index_2d.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/util/tile_matrix.hpp"
#include "tactile/core/debug/assert.hpp"
#include "tactile/core/entity/entity.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"

namespace tactile::core {

/**
 * Indicates whether an entity is a tile layer.
 *
 * \details
 * Tile layer entities feature the following components. \n
 * - \c CMeta \n
 * - \c CLayer \n
 * - \c CTileLayer \n
 * - Either \c CDenseTileLayer or \c CSparseTileLayer
 *
 * \param registry The associated registry.
 * \param id       The identifier of the entity to check.
 *
 * \return
 * True if the entity is a tile layer; false otherwise.
 */
[[nodiscard]]
auto is_tile_layer(const Registry& registry, EntityID id) -> bool;

/**
 * Creates an empty (dense) tile layer.
 *
 * \param registry The associated registry.
 * \param extent   The initial extent.
 *
 * \return
 * A tile layer entity.
 */
[[nodiscard]]
auto make_tile_layer(Registry& registry, const Extent2D& extent) -> EntityID;

/**
 * Destroys a tile layer.
 *
 * \pre The tile layer identifier must be valid.
 *
 * \param registry The associated registry.
 * \param layer_id The identifier of the tile layer to destroy.
 */
void destroy_tile_layer(Registry& registry, EntityID layer_id);

/**
 * Makes a tile layer use a dense tile representation.
 *
 * \details
 * This has no effect if the tile layer is dense.
 *
 * \pre The tile layer identifier must be valid.
 *
 * \param registry The associated registry.
 * \param layer_id The target tile layer.
 */
void convert_to_dense_tile_layer(Registry& registry, EntityID layer_id);

/**
 * Makes a tile layer use a sparse tile representation.
 *
 * \details
 * This has no effect if the tile layer is sparse.
 *
 * \pre The tile layer identifier must be valid.
 *
 * \param registry The associated registry.
 * \param layer_id The target tile layer.
 */
void convert_to_sparse_tile_layer(Registry& registry, EntityID layer_id);

/**
 * Changes the size of a tile layer.
 *
 * \pre The tile layer identifier must be valid.
 *
 * \param registry The associated registry.
 * \param layer_id The target layer identifier.
 * \param extent   The new layer extent.
 */
void resize_tile_layer(Registry& registry, EntityID layer_id, const Extent2D& extent);

/**
 * Serializes the tile data associated with a tile layer as a byte stream.
 *
 * \details
 * This function can be used to obtain the raw tile data suitable for use in for save files
 * that use a non-plain text encoding (and possibly compression).
 *
 * \pre The layer identifier must be valid.
 *
 * \param registry The associated registry.
 * \param layer_id The target layer identifier.
 *
 * \return
 * A stream of tile bytes.
 */
[[nodiscard]]
auto serialize_tile_layer(const Registry& registry, EntityID layer_id) -> ByteStream;

/**
 * Updates a tile at a given position in a tile layer.
 *
 * \details
 * This function has no effect if the tile position is invalid.
 *
 * \pre The layer identifier must be valid.
 *
 * \param registry The associated registry.
 * \param layer_id The target layer identifier.
 * \param index    The target tile position.
 * \param tile_id  The new tile identifier.
 */
void set_layer_tile(Registry& registry,
                    EntityID layer_id,
                    const Index2D& index,
                    TileID tile_id);

/**
 * Returns the value of a tile at a given position in a tile layer.
 *
 * \pre The layer identifier must be valid.
 *
 * \param registry The associated registry.
 * \param layer_id The target layer identifier.
 * \param index    The target tile position.
 *
 * \return
 * A tile identifier if successful; an empty optional otherwise.
 */
[[nodiscard]]
auto get_layer_tile(const Registry& registry, EntityID layer_id, const Index2D& index)
    -> std::optional<TileID>;

/**
 * Visits each tile in a tile layer within a given region.
 *
 * \pre The tile layer identifier must be valid.
 *
 * \tparam T A function object type.
 *
 * \param registry The associated registry.
 * \param layer_id The target tile layer identifier.
 * \param begin    The inclusive first (top-left) tile position.
 * \param end      The exclusive last (bottom-right) tile position.
 * \param callable The function object invoked for each tile in the region.
 */
template <std::invocable<const Index2D&, TileID> T>
constexpr void each_layer_tile(const Registry& registry,
                               const EntityID layer_id,
                               const Index2D& begin,
                               const Index2D& end,
                               const T& callable)
{
  TACTILE_ASSERT(is_tile_layer(registry, layer_id));

  if (const auto& tile_layer = registry.get<CTileLayer>(layer_id);
      !tile_layer.extent.contains(begin) ||
      !tile_layer.extent.contains(Index2D {.x = end.x - 1, .y = end.y - 1})) {
    return;
  }

  if (const auto* dense = registry.find<CDenseTileLayer>(layer_id)) {
    for (auto row = begin.y; row < end.y; ++row) {
      for (auto col = begin.x; col < end.x; ++col) {
        const Index2D index {.x = col, .y = row};
        callable(index, dense->tiles[row][col]);
      }
    }
  }
  else {
    const auto& sparse = registry.get<CSparseTileLayer>(layer_id);
    for (auto row = begin.y; row < end.y; ++row) {
      for (auto col = begin.x; col < end.x; ++col) {
        const Index2D index {.x = col, .y = row};

        const auto iter = sparse.tiles.find(index);
        callable(index, iter != sparse.tiles.end() ? iter->second : kEmptyTile);
      }
    }
  }
}

/**
 * Visits each tile in a tile layer.
 *
 * \pre The tile layer identifier must be valid.
 *
 * \tparam T A function object type.
 *
 * \param registry The associated registry.
 * \param layer_id The target tile layer identifier.
 * \param callable The function object invoked for each tile in the layer.
 */
template <std::invocable<const Index2D&, TileID> T>
constexpr void each_layer_tile(const Registry& registry,
                               const EntityID layer_id,
                               const T& callable)
{
  TACTILE_ASSERT(is_tile_layer(registry, layer_id));
  const auto& tile_layer = registry.get<CTileLayer>(layer_id);

  constexpr Index2D begin {.x = 0, .y = 0};
  const Index2D end {.x = tile_layer.extent.cols, .y = tile_layer.extent.rows};

  each_layer_tile(registry, layer_id, begin, end, callable);
}

}  // namespace tactile::core