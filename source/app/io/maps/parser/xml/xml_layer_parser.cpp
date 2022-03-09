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

#include "xml_layer_parser.hpp"

#include <cstring>  // strcmp
#include <memory>   // make_unique
#include <string>   // string
#include <vector>   // vector

#include "core/utils/strings.hpp"
#include "core/utils/tiles.hpp"
#include "io/maps/xml_utils.hpp"
#include "misc/logging.hpp"
#include "misc/throw.hpp"
#include "xml_attribute_parser.hpp"

namespace tactile::parsing {
namespace {

[[nodiscard]] auto _collect_layer_nodes(pugi::xml_node mapNode)
    -> std::vector<pugi::xml_node>
{
  std::vector<pugi::xml_node> nodes;

  for (auto node : mapNode.children()) {
    if (std::strcmp(node.name(), "layer") == 0 ||
        std::strcmp(node.name(), "objectgroup") == 0 ||
        std::strcmp(node.name(), "group") == 0) {
      nodes.push_back(node);
    }
  }

  return nodes;
}

[[nodiscard]] auto _parse_csv_tiles(const char* csv, ir::TileLayerData& layerData)
    -> parse_error
{
  usize index{};
  for (const auto& token : split(csv, ',')) {
    if (const auto id = from_string<int32>(token.c_str())) {
      const auto [row, col] = to_matrix_coords(index, layerData.col_count);
      layerData.tiles[row][col] = *id;

      ++index;
    }
    else {
      return parse_error::corrupt_tile_layer_data;
    }
  }

  return parse_error::none;
}

[[nodiscard]] auto _parse_tile_nodes(pugi::xml_node dataNode,
                                     ir::TileLayerData& layerData) -> parse_error
{
  usize index = 0;
  for (const auto tileNode : dataNode.children("tile")) {
    const auto [row, col] = to_matrix_coords(index, layerData.col_count);
    layerData.tiles[row][col] = tileNode.attribute("gid").as_int(empty_tile);

    ++index;
  }

  return parse_error::none;
}

[[nodiscard]] auto _parse_tile_data(pugi::xml_node layerNode,
                                    ir::TileLayerData& layerData) -> parse_error
{
  const auto data = layerNode.child("data");

  if (data.empty() || data.text().empty()) {
    return parse_error::no_tile_layer_data;
  }

  /* The encoding attribute is optional, if it is missing then the tile data is
     stored as individual "tile" nodes. */
  if (const auto* encoding = data.attribute("encoding").as_string(nullptr)) {
    /* The only explicit encoding we support is CSV */
    if (std::strcmp(encoding, "csv") != 0) {
      return parse_error::unsupported_tile_layer_encoding;
    }
    else {
      const auto text = data.text();
      if (const auto error = _parse_csv_tiles(text.get(), layerData);
          error != parse_error::none) {
        return error;
      }
    }
  }
  else {
    if (const auto error = _parse_tile_nodes(data, layerData);
        error != parse_error::none) {
      return error;
    }
  }

  return parse_error::none;
}

[[nodiscard]] auto _parse_tile_layer(pugi::xml_node layerNode,
                                     ir::LayerData& layerData,
                                     const usize rows,
                                     const usize columns) -> parse_error
{
  auto& tileLayerData = layerData.data.emplace<ir::TileLayerData>();

  if (const auto width = uint_attribute(layerNode, "width")) {
    tileLayerData.col_count = *width;

    if (tileLayerData.col_count != columns) {
      log_warning("XML tile layer width does not match map width, '{}' vs '{}'",
                  tileLayerData.col_count,
                  columns);
    }
  }
  else {
    log_warning("XML tile layer has no width information, assuming map width...");
    tileLayerData.col_count = columns;
  }

  if (const auto height = uint_attribute(layerNode, "height")) {
    tileLayerData.row_count = *height;

    if (tileLayerData.row_count != rows) {
      log_warning("XML tile layer height does not match map height, '{}' vs '{}'",
                  tileLayerData.row_count,
                  rows);
    }
  }
  else {
    log_warning("XML tile layer has no height information, assuming map height...");
    tileLayerData.row_count = rows;
  }

  tileLayerData.tiles =
      make_tile_matrix(tileLayerData.row_count, tileLayerData.col_count);

  if (const auto err = _parse_tile_data(layerNode, tileLayerData);
      err != parse_error::none) {
    return err;
  }

  return parse_error::none;
}

[[nodiscard]] auto _parse_object_layer(pugi::xml_node layerNode, ir::LayerData& layerData)
    -> parse_error
{
  auto& objectLayerData = layerData.data.emplace<ir::ObjectLayerData>();

  for (const auto objectNode : layerNode.children("object")) {
    auto& objectData = objectLayerData.objects.emplace_back();
    if (const auto err = parse_object(objectNode, objectData); err != parse_error::none) {
      return err;
    }
  }

  return parse_error::none;
}

[[nodiscard]] auto _parse_layer(pugi::xml_node layerNode,
                                ir::LayerData& layerData,
                                const usize index,
                                const usize rows,
                                const usize columns) -> parse_error
{
  layerData.index = index;

  if (const auto id = int_attribute(layerNode, "id")) {
    layerData.id = *id;
  }
  else {
    return parse_error::no_layer_id;
  }

  layerData.name = layerNode.attribute("name").as_string("Layer");
  layerData.opacity = layerNode.attribute("opacity").as_float(1.0f);
  layerData.visible = layerNode.attribute("visible").as_bool(true);

  if (std::strcmp(layerNode.name(), "layer") == 0) {
    layerData.type = LayerType::tile_layer;
    if (const auto err = _parse_tile_layer(layerNode, layerData, rows, columns);
        err != parse_error::none) {
      return err;
    }
  }
  else if (std::strcmp(layerNode.name(), "objectgroup") == 0) {
    layerData.type = LayerType::object_layer;
    if (const auto err = _parse_object_layer(layerNode, layerData);
        err != parse_error::none) {
      return err;
    }
  }
  else if (std::strcmp(layerNode.name(), "group") == 0) {
    layerData.type = LayerType::group_layer;
    auto& groupData = layerData.data.emplace<ir::GroupLayerData>();

    usize childIndex = 0;
    for (auto childLayerNode : _collect_layer_nodes(layerNode)) {
      auto& childLayerData =
          groupData.children.emplace_back(std::make_unique<ir::LayerData>());

      if (const auto err =
              _parse_layer(childLayerNode, *childLayerData, childIndex, rows, columns);
          err != parse_error::none) {
        return err;
      }

      ++childIndex;
    }
  }
  else {
    /* If we enter this branch, then the layer collection is broken */
    throw_traced(tactile_error{"Collected invalid layer node!"});
  }

  if (const auto err = parse_properties(layerNode, layerData.context);
      err != parse_error::none) {
    return err;
  }

  return parse_error::none;
}

}  // namespace

auto parse_object(pugi::xml_node objectNode, ir::ObjectData& objectData) -> parse_error
{
  if (const auto id = int_attribute(objectNode, "id")) {
    objectData.id = *id;
  }
  else {
    return parse_error::no_object_id;
  }

  objectData.name = objectNode.attribute("name").as_string("");
  objectData.tag = objectNode.attribute("type").as_string("");

  objectData.x = objectNode.attribute("x").as_float(0);
  objectData.y = objectNode.attribute("y").as_float(0);
  objectData.width = objectNode.attribute("width").as_float(0);
  objectData.height = objectNode.attribute("height").as_float(0);

  objectData.visible = objectNode.attribute("visible").as_bool(true);

  if (!objectNode.child("point").empty()) {
    objectData.type = object_type::point;
  }
  else if (!objectNode.child("ellipse").empty()) {
    objectData.type = object_type::ellipse;
  }
  else {
    objectData.type = object_type::rect;
  }

  if (const auto err = parse_properties(objectNode, objectData.context);
      err != parse_error::none) {
    return err;
  }

  return parse_error::none;
}

auto parse_layers(pugi::xml_node mapNode, ir::MapData& mapData) -> parse_error
{
  usize index = 0;
  for (const auto layerNode : _collect_layer_nodes(mapNode)) {
    auto& layerData = mapData.layers.emplace_back();

    if (const auto err = _parse_layer(layerNode,
                                      layerData,
                                      index,
                                      mapData.row_count,
                                      mapData.col_count);
        err != parse_error::none) {
      return err;
    }

    ++index;
  }

  return parse_error::none;
}

}  // namespace tactile::parsing
