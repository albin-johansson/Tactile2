/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2023 Albin Johansson
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

#include "tiled_json_exporter.hpp"

#include <utility>  // move

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "core/functional/invoke.hpp"
#include "io/base64_tiles.hpp"
#include "io/export/tiled_info.hpp"
#include "io/ir/map/map_ir.hpp"
#include "io/json_utils.hpp"
#include "tactile/core/io/filesystem.hpp"

namespace tactile {
namespace {

[[nodiscard]] auto _emit_properties(const ContextIR& context) -> JSON
{
  auto array = JSON::array();

  for (const auto& [name, value]: context.properties) {
    auto json = JSON::object();

    json["name"] = name;
    json["type"] = value.get_type();
    json["value"] = value;

    array += json;
  }

  return array;
}

[[nodiscard]] auto _emit_object(const ObjectIR& object) -> JSON
{
  auto json = JSON::object();

  json["id"] = object.id;
  json["name"] = object.name;
  json["x"] = object.pos.x;
  json["y"] = object.pos.y;
  json["width"] = object.size.x;
  json["height"] = object.size.y;
  json["visible"] = object.visible;
  json["type"] = object.tag;
  json["rotation"] = 0;

  switch (object.type) {
    case ObjectType::Rect:
      // Assumed to be rectangle if no point/ellipse indicators
      break;

    case ObjectType::Point: {
      json["point"] = true;
      break;
    }
    case ObjectType::Ellipse: {
      json["ellipse"] = true;
      break;
    }
  }

  if (!object.context.properties.empty()) {
    json["properties"] = _emit_properties(object.context);
  }

  return json;
}

void _emit_tile_layer(JSON& json, const MapIR& map, const LayerIR& layer)
{
  const auto& tile_layer = layer.as_tile_layer();

  json["type"] = "tilelayer";
  json["width"] = map.extent.cols;
  json["height"] = map.extent.rows;

  switch (map.tile_format.encoding) {
    case TileEncoding::Base64: {
      json["encoding"] = "base64";
      break;
    }
    default:
      // Do nothing
      break;
  }

  switch (map.tile_format.compression) {
    case TileCompression::Zlib: {
      json["compression"] = "zlib";
      break;
    }
    case TileCompression::Zstd: {
      json["compression"] = "zstd";
      break;
    }
    default:
      // Do nothing
      break;
  }

  if (map.tile_format.encoding == TileEncoding::Base64) {
    json["data"] =
        base64_encode_tiles(tile_layer.tiles, map.extent, map.tile_format.compression);
  }
  else {
    auto tiles = JSON::array();

    invoke_mn(map.extent.rows, map.extent.cols, [&](const usize row, const usize col) {
      tiles += tile_layer.tiles[row][col];
    });

    json["data"] = std::move(tiles);
  }
}

void _emit_object_layer(JSON& json, const LayerIR& layer)
{
  const auto& object_layer = layer.as_object_layer();

  json["type"] = "objectgroup";

  auto objects = JSON::array();

  for (const auto& object: object_layer.objects) {
    objects += _emit_object(object);
  }

  json["objects"] = std::move(objects);
}

[[nodiscard]] auto _emit_layer(const MapIR& map, const LayerIR& layer) -> JSON
{
  auto json = JSON::object();

  json["id"] = layer.id;
  json["name"] = layer.name;
  json["opacity"] = layer.opacity;
  json["visible"] = layer.visible;
  json["x"] = 0;
  json["y"] = 0;

  switch (layer.type) {
    case LayerType::TileLayer:
      _emit_tile_layer(json, map, layer);
      break;

    case LayerType::ObjectLayer:
      _emit_object_layer(json, layer);
      break;

    case LayerType::GroupLayer: {
      const auto& group_layer = layer.as_group_layer();

      json["type"] = "group";

      auto layers = JSON::array();

      for (const auto& child_layer: group_layer.children) {
        layers += _emit_layer(map, *child_layer);
      }

      json["layers"] = std::move(layers);
      break;
    }
  }

  if (!layer.context.properties.empty()) {
    json["properties"] = _emit_properties(layer.context);
  }

  return json;
}

[[nodiscard]] auto _emit_layers(const MapIR& map) -> JSON
{
  auto array = JSON::array();

  for (const auto& layer: map.layers) {
    array += _emit_layer(map, layer);
  }

  return array;
}

[[nodiscard]] auto _emit_fancy_tile_animation(const TileIR& tile) -> JSON
{
  auto array = JSON::array();

  for (const auto& frame: tile.frames) {
    auto json = JSON::object();

    json["tileid"] = frame.tile_index;
    json["duration"] = frame.duration_ms;

    array += json;
  }

  return array;
}

[[nodiscard]] auto _emit_fancy_tile(const TileID id, const TileIR& tile) -> JSON
{
  auto json = JSON::object();

  json["id"] = id;

  if (!tile.frames.empty()) {
    json["animation"] = _emit_fancy_tile_animation(tile);
  }

  if (!tile.objects.empty()) {
    auto dummy = JSON::object();
    dummy["draworder"] = "index";
    dummy["name"] = "";
    dummy["opacity"] = 1;
    dummy["type"] = "objectgroup";
    dummy["visible"] = true;
    dummy["x"] = 0;
    dummy["y"] = 0;

    auto objects = JSON::array();
    for (const auto& object: tile.objects) {
      objects += _emit_object(object);
    }

    dummy["objects"] = std::move(objects);
    json["objectgroup"] = std::move(dummy);
  }

  if (!tile.context.properties.empty()) {
    json["properties"] = _emit_properties(tile.context);
  }

  return json;
}

[[nodiscard]] auto _emit_fancy_tiles(const TilesetIR& tileset) -> JSON
{
  auto json = JSON::array();

  for (const auto& [id, tile_data]: tileset.fancy_tiles) {
    json += _emit_fancy_tile(id, tile_data);
  }

  return json;
}

void _add_common_tileset_attributes(JSON& json, const Path& dir, const TilesetIR& tileset)
{
  json["name"] = tileset.name;
  json["columns"] = tileset.column_count;

  json["tilewidth"] = tileset.tile_size.x;
  json["tileheight"] = tileset.tile_size.y;
  json["tilecount"] = tileset.tile_count;

  const auto image_path = fs::relative(tileset.image_path, dir);
  json["image"] = to_forward_slashes_path(image_path);

  json["imagewidth"] = tileset.image_size.x;
  json["imageheight"] = tileset.image_size.y;

  json["margin"] = 0;
  json["spacing"] = 0;

  if (!tileset.fancy_tiles.empty()) {
    json["tiles"] = _emit_fancy_tiles(tileset);
  }

  if (!tileset.context.properties.empty()) {
    json["properties"] = _emit_properties(tileset.context);
  }
}

[[nodiscard]] auto _emit_embedded_tileset(const Path& dir, const TilesetIR& tileset)
    -> JSON
{
  auto json = JSON::object();

  json["firstgid"] = tileset.first_tile;
  _add_common_tileset_attributes(json, dir, tileset);

  return json;
}

[[nodiscard]] auto _emit_external_tileset(const TilesetIR& tileset) -> JSON
{
  auto json = JSON::object();

  json["firstgid"] = tileset.first_tile;
  json["source"] = fmt::format("{}.json", tileset.name);

  return json;
}

void _create_external_tileset_file(const Path& dir,
                                   const TilesetIR& tileset,
                                   const Settings& settings)
{
  auto json = JSON::object();
  _add_common_tileset_attributes(json, dir, tileset);

  json["type"] = "tileset";
  json["tiledversion"] = kTiledVersion;
  json["version"] = kTiledJsonFormatVersion;

  const auto name = fmt::format("{}.json", tileset.name);
  const auto path = dir / name;
  const auto indentation = settings.test_flag(SETTINGS_INDENT_OUTPUT_BIT) ? 2 : 0;

  if (save_json_to_file(json, path, indentation).failed()) {
    spdlog::error("[IO] Could not save JSON tileset file");
  }
}

[[nodiscard]] auto _emit_tileset(const Path& dir,
                                 const TilesetIR& tileset,
                                 const Settings& settings) -> JSON
{
  if (settings.test_flag(SETTINGS_EMBED_TILESETS_BIT)) {
    return _emit_embedded_tileset(dir, tileset);
  }
  else {
    _create_external_tileset_file(dir, tileset, settings);
    return _emit_external_tileset(tileset);
  }
}

[[nodiscard]] auto _emit_tilesets(const Path& dir,
                                  const MapIR& ir_map,
                                  const Settings& settings) -> JSON
{
  auto json = JSON::array();

  for (const auto& ir_tileset: ir_map.tilesets) {
    json += _emit_tileset(dir, ir_tileset, settings);
  }

  return json;
}

}  // namespace

void save_map_as_tiled_json(const Path& destination,
                            const MapIR& ir_map,
                            const Settings& settings)
{
  auto json = JSON::object();

  if (!ir_map.component_definitions.empty()) {
    spdlog::warn("[IO] Component data is ignored when saving JSON maps");
  }

  json["type"] = "map";
  json["width"] = ir_map.extent.cols;
  json["height"] = ir_map.extent.rows;

  json["tilewidth"] = ir_map.tile_size.x;
  json["tileheight"] = ir_map.tile_size.y;

  json["nextlayerid"] = ir_map.next_layer_id;
  json["nextobjectid"] = ir_map.next_object_id;

  json["infinite"] = false;
  json["orientation"] = "orthogonal";
  json["renderorder"] = "right-down";

  if (const auto& format = ir_map.tile_format; format.zlib_compression_level) {
    json["compressionlevel"] = *format.zlib_compression_level;
  }
  else if (format.zstd_compression_level) {
    json["compressionlevel"] = *format.zstd_compression_level;
  }
  else {
    json["compressionlevel"] = -1;
  }

  json["tiledversion"] = kTiledVersion;
  json["version"] = kTiledJsonFormatVersion;

  json["tilesets"] = _emit_tilesets(destination.parent_path(), ir_map, settings);
  json["layers"] = _emit_layers(ir_map);

  if (!ir_map.context.properties.empty()) {
    json["properties"] = _emit_properties(ir_map.context);
  }

  if (save_json_to_file(json, destination).failed()) {
    spdlog::error("[IO] Could not save JSON map");
  }
}

}  // namespace tactile
