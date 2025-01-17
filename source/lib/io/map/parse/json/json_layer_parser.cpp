// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include <spdlog/spdlog.h>

#include "core/layer/tile_format.hpp"
#include "core/tile/tile_matrix.hpp"
#include "io/ir/map/map_ir.hpp"
#include "io/map/parse/json/json_parser.hpp"
#include "io/util/base64_tiles.hpp"

namespace tactile {

NLOHMANN_JSON_SERIALIZE_ENUM(CompressionType,
                             {
                               {CompressionType::kNone, ""},
                               {CompressionType::kZlib, "zlib"},
                               {CompressionType::kZstd, "zstd"},
                             })

}  // namespace tactile

namespace tactile {
namespace {

[[nodiscard]] auto parse_object_layer(const JSON& json, LayerIR& layer_data) -> ParseError
{
  auto& object_layer_data = layer_data.data.emplace<ObjectLayerIR>();

  if (const auto iter = json.find("objects"); iter != json.end()) {
    object_layer_data.objects.reserve(iter->size());

    for (const auto& [_, value] : iter->items()) {
      auto& object_data = object_layer_data.objects.emplace_back();
      if (const auto err = parse_object(value, object_data); err != ParseError::None) {
        return err;
      }
    }
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_tile_layer_csv_data(const JSON& json, TileLayerIR& tile_layer)
    -> ParseError
{
  if (!json.is_array()) {
    return ParseError::CorruptTileLayerData;
  }

  usize index = 0;
  for (const auto& [_, value] : json.items()) {
    if (value.is_number_integer()) {
      const auto [row, col] = to_matrix_coords(index, tile_layer.extent.cols);
      tile_layer.tiles[row][col] = value.get<TileID>();
      ++index;
    }
    else {
      return ParseError::CorruptTileLayerData;
    }
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_tile_layer_data(const JSON& json,
                                         MapIR& map,
                                         TileLayerIR& tile_layer) -> ParseError
{
  if (!json.contains("data")) {
    return ParseError::NoTileLayerData;
  }

  const auto data = json.at("data");
  const auto encoding = as_string(json, "encoding").value_or("csv");

  if (encoding == "csv") {
    map.tile_format.encoding = TileEncoding::kPlainText;
    map.tile_format.compression = CompressionType::kNone;

    if (const auto err = parse_tile_layer_csv_data(data, tile_layer);
        err != ParseError::None) {
      return err;
    }
  }
  else if (encoding == "base64") {
    const auto compression = json.at("compression").get<CompressionType>();

    map.tile_format.encoding = TileEncoding::kBase64;
    map.tile_format.compression = compression;

    const auto data_str = data.get<std::string>();
    tile_layer.tiles = base64_decode_tiles(StringView {data_str.data(), data_str.size()},
                                           tile_layer.extent,
                                           compression);
  }
  else {
    return ParseError::UnsupportedTileLayerEncoding;
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_tile_layer(const JSON& json, MapIR& map, LayerIR& layer)
    -> ParseError
{
  auto& tile_layer = layer.data.emplace<TileLayerIR>();

  if (const auto width = as_uint(json, "width")) {
    tile_layer.extent.cols = *width;

    if (tile_layer.extent.cols != map.extent.cols) {
      spdlog::warn("JSON tile layer width does not match map width, '{}' vs '{}'",
                   tile_layer.extent.cols,
                   map.extent.cols);
    }
  }
  else {
    spdlog::warn("JSON tile layer has no width information, assuming map width...");
    tile_layer.extent.cols = map.extent.cols;
  }

  if (const auto height = as_uint(json, "height")) {
    tile_layer.extent.rows = *height;

    if (tile_layer.extent.rows != map.extent.rows) {
      spdlog::warn("JSON tile layer height does not match map height, '{}' vs '{}'",
                   tile_layer.extent.rows,
                   map.extent.rows);
    }
  }
  else {
    spdlog::warn("JSON tile layer has no height information, assuming map height...");
    tile_layer.extent.rows = map.extent.rows;
  }

  tile_layer.tiles = make_tile_matrix(tile_layer.extent);

  if (const auto err = parse_tile_layer_data(json, map, tile_layer);
      err != ParseError::None) {
    return err;
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_layer(const JSON& json,
                               MapIR& map,
                               LayerIR& layer,
                               const usize index) -> ParseError
{
  layer.index = index;

  if (const auto id = as_int(json, "id")) {
    layer.id = *id;
  }
  else {
    return ParseError::NoLayerId;
  }

  layer.name = as_string(json, "name").value_or("Layer");
  layer.opacity = as_float(json, "opacity").value_or(1.0f);
  layer.visible = as_bool(json, "visible").value_or(true);

  if (auto type = as_string(json, "type")) {
    if (type == "tilelayer") {
      layer.type = LayerType::TileLayer;
      if (const auto err = parse_tile_layer(json, map, layer); err != ParseError::None) {
        return err;
      }
    }
    else if (type == "objectgroup") {
      layer.type = LayerType::ObjectLayer;
      if (const auto err = parse_object_layer(json, layer); err != ParseError::None) {
        return err;
      }
    }
    else if (type == "group") {
      layer.type = LayerType::GroupLayer;
      auto& group_layer = layer.data.emplace<GroupLayerIR>();

      usize child_index = 0;
      for (const auto& [_, value] : json.at("layers").items()) {
        auto& child_layer =
            group_layer.children.emplace_back(std::make_unique<LayerIR>());

        if (const auto err = parse_layer(value, map, *child_layer, child_index);
            err != ParseError::None) {
          return err;
        }

        ++child_index;
      }
    }
    else {
      return ParseError::UnsupportedLayerType;
    }
  }
  else {
    return ParseError::NoLayerType;
  }

  if (const auto err = parse_properties(json, layer.context); err != ParseError::None) {
    return err;
  }

  return ParseError::None;
}

}  // namespace

auto parse_object(const JSON& json, ObjectIR& object) -> ParseError
{
  if (const auto id = as_int(json, "id")) {
    object.id = *id;
  }
  else {
    return ParseError::NoObjectId;
  }

  object.name = as_string(json, "name").value_or("");
  object.tag = as_string(json, "type").value_or("");

  object.pos[0] = as_float(json, "x").value_or(0.0f);
  object.pos[1] = as_float(json, "y").value_or(0.0f);
  object.size[0] = as_float(json, "width").value_or(0.0f);
  object.size[1] = as_float(json, "height").value_or(0.0f);

  object.visible = as_bool(json, "visible").value_or(true);

  if (json.contains("point")) {
    object.type = ObjectType::Point;
  }
  else if (json.contains("ellipse")) {
    object.type = ObjectType::Ellipse;
  }
  else {
    object.type = ObjectType::Rect;
  }

  if (const auto err = parse_properties(json, object.context); err != ParseError::None) {
    return err;
  }

  return ParseError::None;
}

auto parse_layers(const JSON& json, MapIR& map) -> ParseError
{
  const auto iter = json.find("layers");

  if (iter == json.end()) {
    spdlog::warn("JSON map has no \"layers\" attribute, which is required!");
    return ParseError::None;
  }

  map.layers.reserve(iter->size());

  usize index = 0;
  for (const auto& [key, value] : iter->items()) {
    auto& layer_data = map.layers.emplace_back();

    if (const auto err = parse_layer(value, map, layer_data, index);
        err != ParseError::None) {
      return err;
    }

    ++index;
  }

  return ParseError::None;
}

}  // namespace tactile
