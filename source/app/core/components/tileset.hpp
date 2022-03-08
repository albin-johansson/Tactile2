/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <filesystem>     // path
#include <unordered_map>  // unordered_map

#include <centurion.hpp>
#include <entt/entt.hpp>

#include "core/region.hpp"
#include "tactile.hpp"

namespace tactile::comp {

/**
 * \brief Tileset state for a single map.
 */
struct tileset_context final
{
  tileset_id next_id{};    ///< Next available tileset ID.
  tile_id next_tile_id{};  ///< Next available global tile ID.

  /**
   * \brief Maps all global tile identifiers to the associated tileset.
   */
  std::unordered_map<tile_id, entt::entity> tile_to_tileset;
};

struct tileset final
{
  tileset_id id{};       ///< Unique ID associated with the tileset.
  tile_id first_id{};    ///< First associated global tile ID.
  tile_id last_id{};     ///< Last associated global tile ID (inclusive).
  int32 tile_width{};    ///< Width of tiles in the tileset.
  int32 tile_height{};   ///< Height of tiles in the tileset.
  int32 tile_count{};    ///< Number of tiles associated with the tileset.
  int32 row_count{};     ///< Amount of tile rows.
  int32 column_count{};  ///< Amount of tile columns.
};

/**
 * \brief Cached information about a single tileset.
 */
struct tileset_cache final
{
  std::unordered_map<tile_id, cen::irect> source_rects;  ///< Tileset source rectangles.
  std::unordered_map<tile_id, entt::entity> tiles;       ///< Additional tile info.

  /**
   * \brief Frame-by-frame cache that maps tiles to the tile that should be rendered.
   */
  mutable std::unordered_map<tile_id, tile_id> source_to_render;
};

struct tileset_selection final
{
  maybe<Region> region;
};

struct active_tileset final
{
  entt::entity entity{entt::null};
};

}  // namespace tactile::comp
