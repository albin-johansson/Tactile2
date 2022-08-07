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

#include <string>   // string
#include <utility>  // move

#include "core/common/fs.hpp"
#include "core/common/ints.hpp"
#include "io/map/ir/ir.hpp"
#include "io/map/parse/yaml/yaml_parser.hpp"
#include "io/util/yaml.hpp"

namespace tactile::io {
namespace {

constexpr int32 tileset_format_version = 1;

[[nodiscard]] auto parse_animation_frame(const YAML::Node& node, ir::MetaTileData& tile)
    -> ParseError
{
  TileIndex tile_index {};
  if (!read_attribute(node, "tile", tile_index)) {
    return ParseError::NoAnimationFrameTile;
  }

  uint64 duration {};
  if (!read_attribute(node, "duration", duration)) {
    return ParseError::NoAnimationFrameDuration;
  }

  auto& frame = tile.frames.emplace_back();
  frame.local_id = tile_index;
  frame.duration_ms = duration;

  return ParseError::None;
}

[[nodiscard]] auto parse_fancy_tile(const YAML::Node&  node,
                                    const ir::MapData& map,
                                    ir::TilesetData&   tileset) -> ParseError
{
  TileID id {};
  if (!read_attribute(node, "id", id)) {
    return ParseError::NoFancyTileId;
  }

  auto& tile = tileset.fancy_tiles[id];

  if (auto sequence = node["animation"]) {
    tile.frames.reserve(sequence.size());

    for (const auto& frame : sequence) {
      if (const auto err = parse_animation_frame(frame, tile); err != ParseError::None) {
        return err;
      }
    }
  }

  if (auto sequence = node["objects"]) {
    tile.objects.reserve(sequence.size());

    for (const auto& object_node : sequence) {
      auto& object = tile.objects.emplace_back();
      if (const auto err = parse_object(object_node, map, object);
          err != ParseError::None) {
        return err;
      }
    }
  }

  if (const auto err = parse_properties(node, tile.context); err != ParseError::None) {
    return err;
  }

  if (const auto err = parse_components(node, map, tile.context);
      err != ParseError::None) {
    return err;
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_fancy_tiles(const YAML::Node&  sequence,
                                     const ir::MapData& map,
                                     ir::TilesetData&   tileset) -> ParseError
{
  tileset.fancy_tiles.reserve(sequence.size());

  for (const auto& node : sequence) {
    if (const auto err = parse_fancy_tile(node, map, tileset); err != ParseError::None) {
      return err;
    }
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_tileset(const fs::path& source,
                                 ir::MapData&    map,
                                 const TileID    first_tile_id) -> ParseError
{
  try {
    const auto node = YAML::LoadFile(source.string());
    if (!node) {
      return ParseError::UnknownExternalTilesetError;
    }

    const auto dir = source.parent_path();

    auto& tileset = map.tilesets.emplace_back();
    tileset.first_tile = first_tile_id;

    int32 version {};
    if (!read_attribute(node, "version", version)) {
      return ParseError::NoTilesetVersion;
    }

    if (version != tileset_format_version) {
      return ParseError::UnsupportedTilesetVersion;
    }

    if (!read_attribute(node, "name", tileset.name)) {
      return ParseError::NoTilesetName;
    }

    if (!read_attribute(node, "tile-count", tileset.tile_count)) {
      return ParseError::NoTilesetTileCount;
    }

    if (!read_attribute(node, "tile-width", tileset.tile_size.x)) {
      return ParseError::NoTilesetTileWidth;
    }

    if (!read_attribute(node, "tile-height", tileset.tile_size.y)) {
      return ParseError::NoTilesetTileHeight;
    }

    if (!read_attribute(node, "column-count", tileset.column_count)) {
      return ParseError::NoTilesetColumnCount;
    }

    std::string relative;
    if (!read_attribute(node, "image-path", relative)) {
      return ParseError::NoTilesetImagePath;
    }

    auto absolute = fs::weakly_canonical(dir / relative);
    if (fs::exists(absolute)) {
      tileset.image_path = std::move(absolute);
    }
    else {
      return ParseError::TilesetImageDoesNotExist;
    }

    if (!read_attribute(node, "image-width", tileset.image_size.x)) {
      return ParseError::NoTilesetImageWidth;
    }

    if (!read_attribute(node, "image-height", tileset.image_size.y)) {
      return ParseError::NoTilesetImageHeight;
    }

    if (auto sequence = node["tiles"]) {
      if (const auto err = parse_fancy_tiles(sequence, map, tileset);
          err != ParseError::None) {
        return err;
      }
    }

    if (const auto err = parse_properties(node, tileset.context);
        err != ParseError::None) {
      return err;
    }

    if (const auto err = parse_components(node, map, tileset.context);
        err != ParseError::None) {
      return err;
    }

    return ParseError::None;
  }
  catch (...) {
    return ParseError::UnknownExternalTilesetError;
  }
}

}  // namespace

auto parse_tilesets(const YAML::Node& sequence, ir::MapData& map, const fs::path& dir)
    -> ParseError
{
  map.tilesets.reserve(sequence.size());

  for (const auto& node : sequence) {
    TileID first {};
    if (!read_attribute(node, "first-global-id", first)) {
      return ParseError::NoTilesetFirstTileId;
    }

    std::string path;
    if (!read_attribute(node, "path", path)) {
      return ParseError::NoExternalTilesetPath;
    }

    const auto source = fs::weakly_canonical(dir / path);

    if (fs::exists(source)) {
      if (const auto err = parse_tileset(source, map, first); err != ParseError::None) {
        return err;
      }
    }
    else {
      return ParseError::ExternalTilesetDoesNotExist;
    }
  }

  return ParseError::None;
}

}  // namespace tactile::io