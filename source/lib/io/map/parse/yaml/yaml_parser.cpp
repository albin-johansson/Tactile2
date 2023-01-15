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

#include "yaml_parser.hpp"

#include <string>   // string
#include <utility>  // move

#include <yaml-cpp/yaml.h>

#include "common/type/expected.hpp"
#include "io/util/yaml.hpp"

namespace tactile::io {
namespace {

[[nodiscard]] auto parse_tile_format(const YAML::Node& node)
    -> Expected<ir::TileFormatData, ParseError>
{
  ir::TileFormatData format;

  if (auto encoding = node["encoding"]) {
    const auto encoding_str = encoding.as<std::string>();
    if (encoding_str == "plain") {
      format.encoding = TileEncoding::Plain;
    }
    else if (encoding_str == "base64") {
      format.encoding = TileEncoding::Base64;
    }
    else {
      return error(ParseError::BadTileFormatEncoding);
    }
  }
  else {
    format.encoding = TileEncoding::Plain;
  }

  if (auto compression = node["compression"]) {
    const auto compression_str = compression.as<std::string>();
    if (compression_str == "none") {
      format.compression = TileCompression::None;
    }
    else if (compression_str == "zlib") {
      format.compression = TileCompression::Zlib;
    }
    else if (compression_str == "zstd") {
      format.compression = TileCompression::Zstd;
    }
    else {
      return error(ParseError::BadTileFormatCompression);
    }
  }
  else {
    format.compression = TileCompression::None;
  }

  read_attribute(node, "zlib-compression-level", format.zlib_compression_level, -1);
  read_attribute(node, "zstd-compression-level", format.zstd_compression_level, 3);

  if (format.encoding == TileEncoding::Plain &&
      format.compression != TileCompression::None) {
    return error(ParseError::PlainEncodingWithCompression);
  }

  if (const auto level = format.zlib_compression_level;
      level && !TileFormat::is_valid_zlib_compression_level(*level)) {
    return error(ParseError::BadZlibCompressionLevel);
  }

  if (const auto level = format.zstd_compression_level;
      level && !TileFormat::is_valid_zstd_compression_level(*level)) {
    return error(ParseError::BadZstdCompressionLevel);
  }

  return format;
}

[[nodiscard]] auto parse_map(const Path& path, ir::MapData& map) -> ParseError
{
  const auto node = YAML::LoadFile(path.string());
  if (!node) {
    return ParseError::CouldNotReadFile;
  }

  if (!read_attribute(node, "row-count", map.extent.rows)) {
    return ParseError::NoMapHeight;
  }

  if (!read_attribute(node, "column-count", map.extent.cols)) {
    return ParseError::NoMapWidth;
  }

  if (!read_attribute(node, "tile-width", map.tile_size.x)) {
    return ParseError::NoMapTileWidth;
  }

  if (!read_attribute(node, "tile-height", map.tile_size.y)) {
    return ParseError::NoMapTileHeight;
  }

  if (!read_attribute(node, "next-layer-id", map.next_layer_id)) {
    return ParseError::NoMapNextLayerId;
  }

  if (!read_attribute(node, "next-object-id", map.next_object_id)) {
    return ParseError::NoMapNextObjectId;
  }

  if (auto format_node = node["tile-format"]) {
    if (const auto format = parse_tile_format(format_node)) {
      map.tile_format = *format;
    }
    else {
      return format.error();
    }
  }

  if (auto components = parse_component_definitions(node)) {
    map.component_definitions = std::move(*components);
  }
  else {
    return components.error();
  }

  if (auto seq = node["tilesets"]) {
    const auto dir = path.parent_path();
    if (auto tilesets = parse_tilesets(seq, map, dir)) {
      map.tilesets = std::move(*tilesets);
    }
    else {
      return tilesets.error();
    }
  }

  if (auto seq = node["layers"]) {
    if (auto layers = parse_layers(seq, map)) {
      map.layers = std::move(*layers);
    }
    else {
      return layers.error();
    }
  }

  if (auto context = parse_context(node, map)) {
    map.context = std::move(*context);
  }
  else {
    return context.error();
  }

  return ParseError::None;
}

}  // namespace

auto parse_yaml_map(const Path& path) -> ParseResult
{
  ParseResult result;
  result.set_path(path);

  const auto error = parse_map(path, result.data());
  result.set_error(error);

  return result;
}

}  // namespace tactile::io