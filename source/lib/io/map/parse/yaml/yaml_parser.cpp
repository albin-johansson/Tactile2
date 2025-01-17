// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "yaml_parser.hpp"

#include <string>   // string
#include <utility>  // move

#include <yaml-cpp/yaml.h>

#include "io/util/yaml.hpp"
#include "tactile/base/container/expected.hpp"

namespace tactile {
namespace {

[[nodiscard]] auto parse_tile_format(const YAML::Node& node)
    -> Expected<TileFormatIR, ParseError>
{
  TileFormatIR format;

  if (auto encoding = node["encoding"]) {
    const auto encoding_str = encoding.as<std::string>();
    if (encoding_str == "plain") {
      format.encoding = TileEncoding::kPlainText;
    }
    else if (encoding_str == "base64") {
      format.encoding = TileEncoding::kBase64;
    }
    else {
      return unexpected(ParseError::BadTileFormatEncoding);
    }
  }
  else {
    format.encoding = TileEncoding::kPlainText;
  }

  if (auto compression = node["compression"]) {
    const auto compression_str = compression.as<std::string>();
    if (compression_str == "none") {
      format.compression = CompressionType::kNone;
    }
    else if (compression_str == "zlib") {
      format.compression = CompressionType::kZlib;
    }
    else if (compression_str == "zstd") {
      format.compression = CompressionType::kZstd;
    }
    else {
      return unexpected(ParseError::BadTileFormatCompression);
    }
  }
  else {
    format.compression = CompressionType::kNone;
  }

  read_attr_or(node, "zlib-compression-level", format.zlib_compression_level, -1);
  read_attr_or(node, "zstd-compression-level", format.zstd_compression_level, 3);

  if (format.encoding == TileEncoding::kPlainText &&
      format.compression != CompressionType::kNone) {
    return unexpected(ParseError::PlainEncodingWithCompression);
  }

  if (const auto level = format.zlib_compression_level;
      level && !TileFormat::is_valid_zlib_compression_level(*level)) {
    return unexpected(ParseError::BadZlibCompressionLevel);
  }

  if (const auto level = format.zstd_compression_level;
      level && !TileFormat::is_valid_zstd_compression_level(*level)) {
    return unexpected(ParseError::BadZstdCompressionLevel);
  }

  return format;
}

[[nodiscard]] auto parse_map(const Path& path, MapIR& map) -> ParseError
{
  const auto node = YAML::LoadFile(path.string());
  if (!node) {
    return ParseError::CouldNotReadFile;
  }

  if (!read_attr(node, "row-count", map.extent.rows)) {
    return ParseError::NoMapHeight;
  }

  if (!read_attr(node, "column-count", map.extent.cols)) {
    return ParseError::NoMapWidth;
  }

  if (!read_attr(node, "tile-width", map.tile_size[0])) {
    return ParseError::NoMapTileWidth;
  }

  if (!read_attr(node, "tile-height", map.tile_size[1])) {
    return ParseError::NoMapTileHeight;
  }

  if (!read_attr(node, "next-layer-id", map.next_layer_id)) {
    return ParseError::NoMapNextLayerId;
  }

  if (!read_attr(node, "next-object-id", map.next_object_id)) {
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
  MapIR ir_map;
  const auto res = parse_map(path, ir_map);

  if (res == ParseError::None) {
    return ir_map;
  }
  else {
    return unexpected(res);
  }
}

}  // namespace tactile
