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

#include <cstring>  // strcmp
#include <string>   // string
#include <vector>   // vector

#include <spdlog/spdlog.h>

#include "core/util/string.hpp"
#include "core/util/tiles.hpp"
#include "io/map/ir/ir.hpp"
#include "io/map/parse/xml/xml_parser.hpp"
#include "io/util/xml.hpp"
#include "misc/panic.hpp"

namespace tactile::io {
namespace {

[[nodiscard]] auto collect_layer_nodes(XMLNode map_node) -> std::vector<XMLNode>
{
  std::vector<XMLNode> nodes;

  for (auto node : map_node.children()) {
    if (std::strcmp(node.name(), "layer") == 0 ||
        std::strcmp(node.name(), "objectgroup") == 0 ||
        std::strcmp(node.name(), "group") == 0) {
      nodes.push_back(node);
    }
  }

  return nodes;
}

[[nodiscard]] auto parse_csv_tiles(const char* csv, ir::TileLayerData& layer)
    -> ParseError
{
  usize index {};
  for (const auto& token : split(csv, ',')) {
    if (const auto id = from_string<int32>(token.c_str())) {
      const auto [row, col] = to_matrix_coords(index, layer.col_count);
      layer.tiles[row][col] = *id;

      ++index;
    }
    else {
      return ParseError::CorruptTileLayerData;
    }
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_tile_nodes(XMLNode data_node, ir::TileLayerData& tile_layer)
    -> ParseError
{
  usize index = 0;
  for (const auto tile_node : data_node.children("tile")) {
    const auto [row, col] = to_matrix_coords(index, tile_layer.col_count);
    tile_layer.tiles[row][col] = tile_node.attribute("gid").as_int(empty_tile);

    ++index;
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_tile_data(XMLNode layer_node, ir::TileLayerData& layer)
    -> ParseError
{
  const auto data = layer_node.child("data");

  if (data.empty() || data.text().empty()) {
    return ParseError::NoTileLayerData;
  }

  if (const auto* encoding = data.attribute("encoding").as_string(nullptr)) {
    if (std::strcmp(encoding, "csv") == 0) {
      const auto text = data.text();
      if (const auto error = parse_csv_tiles(text.get(), layer);
          error != ParseError::None) {
        return error;
      }
    }
    else {
      return ParseError::UnsupportedTileLayerEncoding;
    }
  }
  else {
    if (const auto error = parse_tile_nodes(data, layer); error != ParseError::None) {
      return error;
    }
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_tile_layer(XMLNode        layer_node,
                                    ir::MapData&   map,
                                    ir::LayerData& layer) -> ParseError
{
  auto& tile_layer = layer.data.emplace<ir::TileLayerData>();

  if (const auto width = as_uint(layer_node, "width")) {
    tile_layer.col_count = *width;

    if (tile_layer.col_count != map.col_count) {
      spdlog::warn("XML tile layer width does not match map width, '{}' vs '{}'",
                   tile_layer.col_count,
                   map.col_count);
    }
  }
  else {
    spdlog::warn("XML tile layer has no width information, assuming map width...");
    tile_layer.col_count = map.col_count;
  }

  if (const auto height = as_uint(layer_node, "height")) {
    tile_layer.row_count = *height;

    if (tile_layer.row_count != map.row_count) {
      spdlog::warn("XML tile layer height does not match map height, '{}' vs '{}'",
                   tile_layer.row_count,
                   map.row_count);
    }
  }
  else {
    spdlog::warn("XML tile layer has no height information, assuming map height...");
    tile_layer.row_count = map.row_count;
  }

  tile_layer.tiles = make_tile_matrix(tile_layer.row_count, tile_layer.col_count);

  if (const auto err = parse_tile_data(layer_node, tile_layer); err != ParseError::None) {
    return err;
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_object_layer(XMLNode layer_node, ir::LayerData& layer)
    -> ParseError
{
  auto& object_layer = layer.data.emplace<ir::ObjectLayerData>();

  for (const auto object_node : layer_node.children("object")) {
    auto& object = object_layer.objects.emplace_back();
    if (const auto err = parse_object(object_node, object); err != ParseError::None) {
      return err;
    }
  }

  return ParseError::None;
}

[[nodiscard]] auto parse_layer(XMLNode        layer_node,
                               ir::MapData&   map,
                               ir::LayerData& layer,
                               const usize    index) -> ParseError
{
  layer.index = index;

  if (const auto id = as_int(layer_node, "id")) {
    layer.id = *id;
  }
  else {
    return ParseError::NoLayerId;
  }

  layer.name = layer_node.attribute("name").as_string("Layer");
  layer.opacity = layer_node.attribute("opacity").as_float(1.0f);
  layer.visible = layer_node.attribute("visible").as_bool(true);

  if (std::strcmp(layer_node.name(), "layer") == 0) {
    layer.type = LayerType::TileLayer;
    if (const auto err = parse_tile_layer(layer_node, map, layer);
        err != ParseError::None) {
      return err;
    }
  }
  else if (std::strcmp(layer_node.name(), "objectgroup") == 0) {
    layer.type = LayerType::ObjectLayer;
    if (const auto err = parse_object_layer(layer_node, layer); err != ParseError::None) {
      return err;
    }
  }
  else if (std::strcmp(layer_node.name(), "group") == 0) {
    layer.type = LayerType::GroupLayer;
    auto& group = layer.data.emplace<ir::GroupLayerData>();

    usize child_index = 0;
    for (auto child_node : collect_layer_nodes(layer_node)) {
      auto& child_layer = group.children.emplace_back(std::make_unique<ir::LayerData>());

      if (const auto err = parse_layer(child_node, map, *child_layer, child_index);
          err != ParseError::None) {
        return err;
      }

      ++child_index;
    }
  }
  else {
    // If we enter this branch, then the layer collection is broken
    throw TactileError {"Collected invalid layer node!"};
  }

  if (const auto err = parse_properties(layer_node, layer.context);
      err != ParseError::None) {
    return err;
  }

  return ParseError::None;
}

}  // namespace

auto parse_object(XMLNode object_node, ir::ObjectData& object) -> ParseError
{
  if (const auto id = as_int(object_node, "id")) {
    object.id = *id;
  }
  else {
    return ParseError::NoObjectId;
  }

  object.name = object_node.attribute("name").as_string("");
  object.tag = object_node.attribute("type").as_string("");

  object.pos.x = object_node.attribute("x").as_float(0);
  object.pos.y = object_node.attribute("y").as_float(0);
  object.size.x = object_node.attribute("width").as_float(0);
  object.size.y = object_node.attribute("height").as_float(0);

  object.visible = object_node.attribute("visible").as_bool(true);

  if (!object_node.child("point").empty()) {
    object.type = ObjectType::Point;
  }
  else if (!object_node.child("ellipse").empty()) {
    object.type = ObjectType::Ellipse;
  }
  else {
    object.type = ObjectType::Rect;
  }

  if (const auto err = parse_properties(object_node, object.context);
      err != ParseError::None) {
    return err;
  }

  return ParseError::None;
}

auto parse_layers(XMLNode map_node, ir::MapData& map) -> ParseError
{
  usize index = 0;
  for (const auto layer_node : collect_layer_nodes(map_node)) {
    auto& layer = map.layers.emplace_back();

    if (const auto err = parse_layer(layer_node, map, layer, index);
        err != ParseError::None) {
      return err;
    }

    ++index;
  }

  return ParseError::None;
}

}  // namespace tactile::io
