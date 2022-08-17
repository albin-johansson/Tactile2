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

#include <fstream>  // ofstream
#include <ios>      // ios
#include <sstream>  // stringstream
#include <variant>  // get

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "core/common/fs.hpp"
#include "core/util/filesystem.hpp"
#include "core/util/functional.hpp"
#include "io/map/emit/emit_info.hpp"
#include "io/map/emit/emitter.hpp"
#include "io/map/tiled_info.hpp"
#include "io/persist/preferences.hpp"
#include "io/util/xml.hpp"
#include "misc/panic.hpp"

namespace tactile::io {
namespace {

void append_properties(XMLNode node, const ir::ContextData& context)
{
  if (context.properties.empty()) {
    return;
  }

  auto collection = node.append_child("properties");

  for (const auto& [property_name, property_value] : context.properties) {
    auto property_node = collection.append_child("property");
    property_node.append_attribute("name").set_value(property_name.c_str());

    const auto type = property_value.type();

    // Properties with no type attribute are assumed to be string properties
    if (type != AttributeType::String) {
      property_node.append_attribute("type").set_value(stringify(type));
    }

    auto value_attr = property_node.append_attribute("value");

    switch (type) {
      case AttributeType::String:
        value_attr.set_value(property_value.as_string().c_str());
        break;

      case AttributeType::Int:
        value_attr.set_value(property_value.as_int());
        break;

      case AttributeType::Float:
        value_attr.set_value(property_value.as_float());
        break;

      case AttributeType::Bool:
        value_attr.set_value(property_value.as_bool());
        break;

      case AttributeType::Path: {
        const auto str = convert_to_forward_slashes(property_value.as_path());
        value_attr.set_value(str.c_str());
        break;
      }
      case AttributeType::Color:
        value_attr.set_value(property_value.as_color().as_argb().c_str());
        break;

      case AttributeType::Object:
        value_attr.set_value(property_value.as_object());
        break;
    }
  }
}

void append_object(XMLNode node, const ir::ObjectData& object)
{
  auto object_node = node.append_child("object");
  object_node.append_attribute("id").set_value(object.id);

  if (!object.name.empty()) {
    object_node.append_attribute("name").set_value(object.name.c_str());
  }

  if (!object.tag.empty()) {
    object_node.append_attribute("type").set_value(object.tag.c_str());
  }

  if (object.pos.x != 0.0f) {
    object_node.append_attribute("x").set_value(object.pos.x);
  }

  if (object.pos.y != 0.0f) {
    object_node.append_attribute("y").set_value(object.pos.y);
  }

  if (object.size.x != 0.0f) {
    object_node.append_attribute("width").set_value(object.size.x);
  }

  if (object.size.y != 0.0f) {
    object_node.append_attribute("height").set_value(object.size.y);
  }

  if (!object.visible) {
    object_node.append_attribute("visible").set_value(0);
  }

  // Objects are assumed to be rectangles unless explicitly told otherwise
  if (object.type == ObjectType::Point) {
    object_node.append_child("point");
  }
  else if (object.type == ObjectType::Ellipse) {
    object_node.append_child("ellipse");
  }

  append_properties(object_node, object.context);
}

void append_common_layer_attributes(XMLNode node, const ir::LayerData& layer)
{
  node.append_attribute("id").set_value(layer.id);
  node.append_attribute("name").set_value(layer.name.c_str());

  if (layer.opacity != 1.0f) {
    node.append_attribute("opacity").set_value(layer.opacity);
  }

  if (!layer.visible) {
    node.append_attribute("visible").set_value(0);
  }
}

void append_tile_layer(XMLNode root, const ir::LayerData& layer)
{
  const auto& prefs = get_preferences();
  const auto& tile_layer = std::get<ir::TileLayerData>(layer.data);

  auto node = root.append_child("layer");
  append_common_layer_attributes(node, layer);

  node.append_attribute("width").set_value(tile_layer.col_count);
  node.append_attribute("height").set_value(tile_layer.row_count);

  append_properties(node, layer.context);

  auto data_node = node.append_child("data");
  data_node.append_attribute("encoding").set_value("csv");

  const auto count = tile_layer.row_count * tile_layer.col_count;

  std::stringstream stream;

  invoke_mn(tile_layer.row_count,
            tile_layer.col_count,
            [&, index = 0ull](const usize row, const usize col) mutable {
              if (prefs.fold_tile_data && index == 0) {
                stream << '\n';
              }

              stream << tile_layer.tiles[row][col];
              if (index < count - 1u) {
                stream << ',';
              }

              if (prefs.fold_tile_data && (index + 1) % tile_layer.col_count == 0) {
                stream << '\n';
              }

              ++index;
            });

  data_node.text().set(stream.str().c_str());
}

void append_object_layer(XMLNode root, const ir::LayerData& layer)
{
  const auto& object_layer = std::get<ir::ObjectLayerData>(layer.data);

  auto node = root.append_child("objectgroup");
  append_common_layer_attributes(node, layer);
  append_properties(node, layer.context);

  for (const auto& object : object_layer.objects) {
    append_object(node, object);
  }
}

void append_layer(XMLNode root, const ir::LayerData& layer)
{
  switch (layer.type) {
    case LayerType::TileLayer:
      append_tile_layer(root, layer);
      break;

    case LayerType::ObjectLayer:
      append_object_layer(root, layer);
      break;

    case LayerType::GroupLayer: {
      const auto& group_layer = std::get<ir::GroupLayerData>(layer.data);

      auto collection = root.append_child("group");
      append_common_layer_attributes(collection, layer);
      append_properties(collection, layer.context);

      for (const auto& child_layer : group_layer.children) {
        append_layer(collection, *child_layer);
      }

      break;
    }
    default:
      throw TactileError {"Invalid layer type!"};
  }
}

void append_fancy_tiles(XMLNode node, const ir::TilesetData& tileset)
{
  for (const auto& [id, tile] : tileset.fancy_tiles) {
    auto tile_node = node.append_child("tile");
    tile_node.append_attribute("id").set_value(id);

    if (!tile.frames.empty()) {
      auto animation_node = tile_node.append_child("animation");

      for (const auto& frame : tile.frames) {
        auto frame_node = animation_node.append_child("frame");
        frame_node.append_attribute("tileid").set_value(frame.local_id);
        frame_node.append_attribute("duration").set_value(frame.duration_ms);
      }
    }

    if (!tile.objects.empty()) {
      auto object_layer_node = tile_node.append_child("objectgroup");
      object_layer_node.append_attribute("draworder").set_value("index");

      for (const auto& object : tile.objects) {
        append_object(object_layer_node, object);
      }
    }

    append_properties(tile_node, tile.context);
  }
}

void append_common_tileset_attributes(XMLNode                node,
                                      const ir::TilesetData& tileset,
                                      const fs::path&        dir)
{
  node.append_attribute("name").set_value(tileset.name.c_str());

  node.append_attribute("tilewidth").set_value(tileset.tile_size.x);
  node.append_attribute("tileheight").set_value(tileset.tile_size.y);

  node.append_attribute("tilecount").set_value(tileset.tile_count);
  node.append_attribute("columns").set_value(tileset.column_count);

  {
    auto image_node = node.append_child("image");

    const auto source = convert_to_forward_slashes(fs::relative(tileset.image_path, dir));
    image_node.append_attribute("source").set_value(source.c_str());

    image_node.append_attribute("width").set_value(tileset.image_size.x);
    image_node.append_attribute("height").set_value(tileset.image_size.y);
  }

  append_fancy_tiles(node, tileset);
  append_properties(node, tileset.context);
}

void append_embedded_tileset(XMLNode                root,
                             const ir::TilesetData& tileset,
                             const fs::path&        dir)
{
  auto node = root.append_child("tileset");
  node.append_attribute("firstgid").set_value(tileset.first_tile);

  append_common_tileset_attributes(node, tileset, dir);
}

void append_external_tileset(XMLNode                root,
                             const ir::TilesetData& tileset,
                             const std::string&     filename)
{
  auto node = root.append_child("tileset");
  node.append_attribute("firstgid").set_value(tileset.first_tile);
  node.append_attribute("source").set_value(filename.c_str());
}

void emit_external_tileset_file(const fs::path&        path,
                                const ir::TilesetData& tileset,
                                const fs::path&        dir)
{
  pugi::xml_document document;

  auto root = document.append_child("tileset");
  root.append_attribute("version").set_value(tiled_xml_format_version);
  root.append_attribute("tiledversion").set_value(tiled_version);

  append_common_tileset_attributes(root, tileset, dir);

  std::ofstream stream {path, std::ios::out};
  document.save(stream);
}

void append_tileset(XMLNode root, const ir::TilesetData& tileset, const fs::path& dir)
{
  const auto& prefs = get_preferences();
  if (prefs.embed_tilesets) {
    append_embedded_tileset(root, tileset, dir);
  }
  else {
    const auto filename = fmt::format("{}.tsx", tileset.name);
    const auto source = dir / filename;
    emit_external_tileset_file(source, tileset, dir);
    append_external_tileset(root, tileset, filename);
  }
}

void append_root(pugi::xml_document& document, const EmitInfo& info)
{
  const auto& map = info.data();
  auto        root = document.append_child("map");

  root.append_attribute("version").set_value(tiled_xml_format_version);
  root.append_attribute("tiledversion").set_value(tiled_version);

  root.append_attribute("orientation").set_value("orthogonal");
  root.append_attribute("renderorder").set_value("right-down");
  root.append_attribute("infinite").set_value(0);

  root.append_attribute("tilewidth").set_value(map.tile_size.x);
  root.append_attribute("tileheight").set_value(map.tile_size.y);

  root.append_attribute("width").set_value(map.col_count);
  root.append_attribute("height").set_value(map.row_count);

  root.append_attribute("nextlayerid").set_value(map.next_layer_id);
  root.append_attribute("nextobjectid").set_value(map.next_object_id);

  append_properties(root, map.context);

  for (const auto& tileset : map.tilesets) {
    append_tileset(root, tileset, info.destination_dir());
  }

  for (const auto& layer : map.layers) {
    append_layer(root, layer);
  }
}

}  // namespace

void emit_xml_map(const EmitInfo& info)
{
  if (!info.data().component_definitions.empty()) {
    spdlog::warn("Component data will be ignored when saving the map as XML!");
  }

  pugi::xml_document document;
  append_root(document, info);

  std::ofstream stream {info.destination_file(), std::ios::out | std::ios::trunc};
  document.save(stream, " ");
}

}  // namespace tactile::io