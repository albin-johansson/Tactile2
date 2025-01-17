// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected
#include <optional>  // optional

#include "tactile/base/debug/error_code.hpp"
#include "tactile/base/id.hpp"
#include "tactile/base/render/renderer.hpp"
#include "tactile/core/entity/entity.hpp"

namespace tactile {

namespace ir {
struct TilesetRef;
}  // namespace ir

namespace core {

struct CTexture;
struct TilesetSpec;
struct TileRange;
class Registry;

/**
 * Indicates whether an entity is a tileset.
 *
 * \details
 * Tileset entities feature at least the following components. \n
 * - \c CMeta \n
 * - \c CTileset \n
 * - \c CTexture \n
 * - \c CViewport
 *
 * \param registry The associated registry.
 * \param entity   The entity to check.
 *
 * \return
 * True if the entity is a tileset; false otherwise.
 */
[[nodiscard]]
auto is_tileset(const Registry& registry, EntityID entity) -> bool;

/**
 * Indicates whether an entity is a tileset instance.
 *
 * \param registry The associated registry.
 * \param entity   The entity identifier to check.
 *
 * \return
 * True if the entity is a tileset instance; false otherwise.
 */
[[nodiscard]]
auto is_tileset_instance(const Registry& registry, EntityID entity) -> bool;

/**
 * Creates a tileset.
 *
 * \note
 * The associated texture must have been loaded before calling this function.
 *
 * \param registry The associated registry.
 * \param spec     The tileset specification.
 *
 * \return
 * A tileset entity if successful; an invalid entity otherwise.
 */
[[nodiscard]]
auto make_tileset(Registry& registry, const TilesetSpec& spec) -> EntityID;

/**
 * Creates a tileset instance from an intermediate representation.
 *
 * \param registry       The associated registry.
 * \param renderer       The renderer to use for loading textures.
 * \param ir_tileset_ref The intermediate tileset representation.
 *
 * \return
 * A tileset entity identifier if successful; an error code otherwise.
 *
 * \pre The registry must feature a \c CTileCache context component.
 */
[[nodiscard]]
auto make_tileset(Registry& registry,
                  IRenderer& renderer,
                  const ir::TilesetRef& ir_tileset_ref) -> std::expected<EntityID, ErrorCode>;

/**
 * Initializes a tileset "instance".
 *
 * \details
 * Tileset instances feature the same components as normal tilesets, with the
 * addition of map related information, such as the assigned tile range
 * (see \c CTilesetInstance). This function should be used to initialize
 * tilesets stored in map document registries.
 *
 * \details
 * Tile identifiers are assigned sequentially in the tileset instance.
 * For example, given a tileset with 100 tiles, a tileset instance created
 * with the first tile identifier configured as 10 will claim the identifiers
 * in the interval [10, 110).
 *
 * \details
 * All tiles associated with the tileset will be registered in the \c CTileCache
 * context component of the provided registry.
 *
 * \note
 * The designated tile range must be unique to the tileset.
 *
 * \param registry       The associated registry.
 * \param tileset_entity The target tileset.
 * \param first_tile_id  The first tile identifier to assign to the tileset.
 *
 * \return
 * Nothing if successful; an error code otherwise.
 *
 * \pre The specified entity must be a valid tileset.
 * \pre The registry must feature a \c CTileCache context component.
 */
[[nodiscard]]
auto init_tileset_instance(Registry& registry, EntityID tileset_entity, TileID first_tile_id)
    -> std::expected<void, ErrorCode>;

/**
 * Creates a tileset instance.
 *
 * \param registry      The associated registry.
 * \param spec          The tileset specification.
 * \param first_tile_id The first global tile identifier to associate with the tileset.
 *
 * \return
 * A tileset instance identifier if successful; an error code otherwise.
 *
 * \see \c make_tileset
 * \see \c init_tileset_instance
 */
auto make_tileset_instance(Registry& registry, const TilesetSpec& spec, TileID first_tile_id)
    -> std::expected<EntityID, ErrorCode>;

/**
 * Destroys a tileset and all of its associated tiles.
 *
 * \details
 * If the specified tileset features a \c CTilesetInstance component, then all
 * associated tiles will be unregistered from the \c CTileCache context
 * component in the provided registry.
 *
 * \param registry       The associated registry.
 * \param tileset_entity The tileset to destroy.
 *
 * \pre The specified entity must be a valid tileset.
 * \pre The registry must feature a \c CTileCache context component if the
 *      entity is a tileset instance.
 */
void destroy_tileset(Registry& registry, EntityID tileset_entity);

/**
 * Creates a deep copy of a tileset.
 *
 * \param registry       The associated registry.
 * \param tileset_entity The tileset that will be copied.
 *
 * \return
 * A tileset entity.
 *
 * \pre The specified entity must be a valid tileset.
 */
[[nodiscard]]
auto copy_tileset(Registry& registry, EntityID tileset_entity) -> EntityID;

/**
 * Returns the appearance of a tile in a tileset.
 *
 * \details
 * This function should be used to determine how to render tiles correctly.
 * For non-animated tiles, this function simply returns the given tile index.
 *
 *
 * \param registry       The associated registry.
 * \param tileset_entity The tileset that contains the tile.
 * \param tile_index     The index of the tile to query.
 *
 * \return
 * The index of the tile that should be shown instead of the specified tile.
 *
 * \pre The specified entity must be a valid tileset.
 *
 * \complexity O(1)
 */
[[nodiscard]]
auto get_tile_appearance(const Registry& registry,
                         EntityID tileset_entity,
                         TileIndex tile_index) -> TileIndex;

/**
 * Returns the tileset entity that features a given tile.
 *
 * \complexity O(1).
 *
 * \pre The registry must feature a \c CTileCache context component.
 *
 * \param registry The associated registry.
 * \param tile_id  The tile identifier to look for.
 *
 * \return
 * A tileset entity if a tileset was found; an invalid entity otherwise.
 */
[[nodiscard]]
auto find_tileset(const Registry& registry, TileID tile_id) -> EntityID;

/**
 * Returns the local tile index of the tile associated with a given identifier.
 *
 * \pre The registry must feature a \c CTileCache context component.
 *
 * \param registry The associated registry.
 * \param tile_id  The tile identifier to convert.
 *
 * \return
 * A tile index if successful; nothing otherwise.
 */
[[nodiscard]]
auto get_tile_index(const Registry& registry, TileID tile_id) -> std::optional<TileIndex>;

/**
 * Indicates whether the tiles in a tile range are available for use.
 *
 * \param registry The associated registry.
 * \param range    The tile range to check.
 *
 * \return
 * True if the tile range is available; false otherwise.
 */
[[nodiscard]]
auto is_tile_range_available(const Registry& registry, const TileRange& range) -> bool;

/**
 * Indicates whether a tile range contains a given tile identifier.
 *
 * \param tile_range The tile range.
 * \param tile_id    The tile identifier to look for.
 *
 * \return
 * True if the tile is within the tile range; false otherwise.
 */
[[nodiscard]]
auto has_tile(const TileRange& tile_range, TileID tile_id) -> bool;

}  // namespace core
}  // namespace tactile
