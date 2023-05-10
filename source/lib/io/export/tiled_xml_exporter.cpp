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

#include "tiled_xml_exporter.hpp"

#include <sstream>  // stringstream

#include <fmt/format.h>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "common/debug/panic.hpp"
#include "common/util/filesystem.hpp"
#include "common/util/functional.hpp"
#include "io/export/tiled_info.hpp"
#include "io/ir/map/map_ir.hpp"
#include "io/stream.hpp"
#include "io/util/base64_tiles.hpp"
#include "io/util/xml.hpp"
#include "model/settings.hpp"

namespace tactile {
namespace {

void append_properties(XmlNode node, const ContextIR& context)
{
  if (context.properties.empty()) {
    return;
  }

  auto collection = node.append_child("properties");

  for (const auto& [property_name, property_value]: context.properties) {
    auto property_node = collection.append_child("property");
    property_node.append_attribute("name").set_value(property_name.c_str());

    const auto type = property_value.get_type();

    // Properties with no type attribute are assumed to be string properties
    if (type != AttributeType::String && !property_value.is_any_vector()) {
      property_node.append_attribute("type").set_value(
          serialize_to_save_format(type).data());
    }

    auto value_attr = property_node.append_attribute("value");

    switch (type) {
      case AttributeType::String:
        value_attr.set_value(property_value.as_string().c_str());
        break;

      case AttributeType::Int:
        value_attr.set_value(property_value.as_int());
        break;

      case AttributeType::Int2: {
        const auto str = serialize_to_save_format(property_value.as_int2());
        value_attr.set_value(str.c_str());
        break;
      }
      case AttributeType::Int3: {
        const auto str = serialize_to_save_format(property_value.as_int3());
        value_attr.set_value(str.c_str());
        break;
      }
      case AttributeType::Int4: {
        const auto str = serialize_to_save_format(property_value.as_int4());
        value_attr.set_value(str.c_str());
        break;
      }
      case AttributeType::Float:
        value_attr.set_value(property_value.as_float());
        break;

      case AttributeType::Float2: {
        const auto str = serialize_to_save_format(property_value.as_float2());
        value_attr.set_value(str.c_str());
        break;
      }
      case AttributeType::Float3: {
        const auto str = serialize_to_save_format(property_value.as_float3());
        value_attr.set_value(str.c_str());
        break;
      }
      case AttributeType::Float4: {
        const auto str = serialize_to_save_format(property_value.as_float4());
        value_attr.set_value(str.c_str());
        break;
      }
      case AttributeType::Bool:
        value_attr.set_value(property_value.as_bool());
        break;

      case AttributeType::Path: {
        const auto str = use_forward_slashes(property_value.as_path());
        value_attr.set_value(str.c_str());
        break;
      }
      case AttributeType::Color:
        value_attr.set_value(property_value.as_color().as_argb().c_str());
        break;

      case AttributeType::Object:
        value_attr.set_value(property_value.as_object());
        break;

      default:
        spdlog::warn("Cannot store '{}' property in an XML file",
                     magic_enum::enum_name(type));
        break;
    }
  }
}

void append_object(XmlNode node, const ObjectIR& object)
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

void append_common_layer_attributes(XmlNode node, const LayerIR& layer)
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

void append_csv_tile_layer_data(XmlNode data_node,
                                const MapIR& map,
                                const TileLayerIR& tile_layer)
{
  data_node.append_attribute("encoding").set_value("csv");

  std::stringstream stream;

  const auto tile_count = map.extent.rows * map.extent.cols;

  const auto& settings = get_settings();
  const bool fold_tile_data = settings.test_flag(SETTINGS_FOLD_TILE_DATA_BIT);

  invoke_mn(map.extent.rows,
            map.extent.cols,
            [&, index = 0ull](const usize row, const usize col) mutable {
              if (fold_tile_data && index == 0) {
                stream << '\n';
              }

              stream << tile_layer.tiles[row][col];
              if (index < tile_count - 1u) {
                stream << ',';
              }

              if (fold_tile_data && (index + 1) % tile_layer.extent.cols == 0) {
                stream << '\n';
              }

              ++index;
            });

  data_node.text().set(stream.str().c_str());
}

void append_base64_tile_layer_data(XmlNode data_node,
                                   const MapIR& map,
                                   const TileLayerIR& tile_layer)
{
  data_node.append_attribute("encoding").set_value("base64");

  switch (map.tile_format.compression) {
    case TileCompression::None:
      // Do nothing
      break;

    case TileCompression::Zlib:
      data_node.append_attribute("compression").set_value("zlib");
      break;

    case TileCompression::Zstd:
      data_node.append_attribute("compression").set_value("zstd");
      break;

    default:
      throw TactileError {"Invalid compression strategy!"};
  }

  const auto tile_data = base64_encode_tiles(tile_layer.tiles,
                                             tile_layer.extent,
                                             map.tile_format.compression);
  data_node.text().set(tile_data.c_str());
}

void append_tile_layer(XmlNode root, const MapIR& map, const LayerIR& layer)
{
  const auto& tile_layer = layer.as_tile_layer();

  auto node = root.append_child("layer");
  append_common_layer_attributes(node, layer);

  node.append_attribute("width").set_value(map.extent.cols);
  node.append_attribute("height").set_value(map.extent.rows);

  append_properties(node, layer.context);

  auto data_node = node.append_child("data");
  switch (map.tile_format.encoding) {
    case TileEncoding::Plain:
      append_csv_tile_layer_data(data_node, map, tile_layer);
      break;

    case TileEncoding::Base64:
      append_base64_tile_layer_data(data_node, map, tile_layer);
      break;

    default:
      throw TactileError {"Invalid tile encoding!"};
  }
}

void append_object_layer(XmlNode root, const LayerIR& layer)
{
  const auto& object_layer = layer.as_object_layer();

  auto node = root.append_child("objectgroup");
  append_common_layer_attributes(node, layer);
  append_properties(node, layer.context);

  for (const auto& object: object_layer.objects) {
    append_object(node, object);
  }
}

void append_layer(XmlNode root, const MapIR& map, const LayerIR& layer)
{
  switch (layer.type) {
    case LayerType::TileLayer:
      append_tile_layer(root, map, layer);
      break;

    case LayerType::ObjectLayer:
      append_object_layer(root, layer);
      break;

    case LayerType::GroupLayer: {
      const auto& group_layer = layer.as_group_layer();

      auto collection = root.append_child("group");
      append_common_layer_attributes(collection, layer);
      append_properties(collection, layer.context);

      for (const auto& child_layer: group_layer.children) {
        append_layer(collection, map, *child_layer);
      }

      break;
    }
    default:
      throw TactileError {"Invalid layer type!"};
  }
}

void append_fancy_tiles(XmlNode node, const TilesetIR& tileset)
{
  for (const auto& [id, tile]: tileset.fancy_tiles) {
    auto tile_node = node.append_child("tile");
    tile_node.append_attribute("id").set_value(id);

    if (!tile.frames.empty()) {
      auto animation_node = tile_node.append_child("animation");

      for (const auto& frame: tile.frames) {
        auto frame_node = animation_node.append_child("frame");
        frame_node.append_attribute("tileid").set_value(frame.tile_index);
        frame_node.append_attribute("duration").set_value(frame.duration_ms);
      }
    }

    if (!tile.objects.empty()) {
      auto object_layer_node = tile_node.append_child("objectgroup");
      object_layer_node.append_attribute("draworder").set_value("index");

      for (const auto& object: tile.objects) {
        append_object(object_layer_node, object);
      }
    }

    append_properties(tile_node, tile.context);
  }
}

void append_common_tileset_attributes(XmlNode node,
                                      const TilesetIR& tileset,
                                      const Path& dir)
{
  node.append_attribute("name").set_value(tileset.name.c_str());

  node.append_attribute("tilewidth").set_value(tileset.tile_size.x);
  node.append_attribute("tileheight").set_value(tileset.tile_size.y);

  node.append_attribute("tilecount").set_value(tileset.tile_count);
  node.append_attribute("columns").set_value(tileset.column_count);

  {
    auto image_node = node.append_child("image");

    const auto source = use_forward_slashes(fs::relative(tileset.image_path, dir));
    image_node.append_attribute("source").set_value(source.c_str());

    image_node.append_attribute("width").set_value(tileset.image_size.x);
    image_node.append_attribute("height").set_value(tileset.image_size.y);
  }

  append_fancy_tiles(node, tileset);
  append_properties(node, tileset.context);
}

void append_embedded_tileset(XmlNode root, const TilesetIR& tileset, const Path& dir)
{
  auto node = root.append_child("tileset");
  node.append_attribute("firstgid").set_value(tileset.first_tile);

  append_common_tileset_attributes(node, tileset, dir);
}

void append_external_tileset(XmlNode root,
                             const TilesetIR& tileset,
                             const String& filename)
{
  auto node = root.append_child("tileset");
  node.append_attribute("firstgid").set_value(tileset.first_tile);
  node.append_attribute("source").set_value(filename.c_str());
}

void emit_external_tileset_file(const Path& path,
                                const TilesetIR& tileset,
                                const Path& dir)
{
  pugi::xml_document document;

  auto root = document.append_child("tileset");
  root.append_attribute("version").set_value(kTiledXmlFormatVersion);
  root.append_attribute("tiledversion").set_value(kTiledVersion);

  append_common_tileset_attributes(root, tileset, dir);

  if (save_xml_to_file(document, path).failed()) {
    spdlog::error("Could not save XML tileset file");
  }
}

void append_tileset(XmlNode root, const Path& dir, const TilesetIR& tileset)
{
  const auto& settings = get_settings();
  if (settings.test_flag(SETTINGS_EMBED_TILESETS_BIT)) {
    append_embedded_tileset(root, tileset, dir);
  }
  else {
    const auto filename = fmt::format("{}.tsx", tileset.name);
    const auto source = dir / filename;
    emit_external_tileset_file(source, tileset, dir);
    append_external_tileset(root, tileset, filename);
  }
}

void append_root(pugi::xml_document& document, const Path& dir, const MapIR& ir_map)
{
  auto root = document.append_child("map");

  root.append_attribute("version").set_value(kTiledXmlFormatVersion);
  root.append_attribute("tiledversion").set_value(kTiledVersion);

  root.append_attribute("orientation").set_value("orthogonal");
  root.append_attribute("renderorder").set_value("right-down");
  root.append_attribute("infinite").set_value(0);

  root.append_attribute("tilewidth").set_value(ir_map.tile_size.x);
  root.append_attribute("tileheight").set_value(ir_map.tile_size.y);

  root.append_attribute("width").set_value(ir_map.extent.cols);
  root.append_attribute("height").set_value(ir_map.extent.rows);

  root.append_attribute("nextlayerid").set_value(ir_map.next_layer_id);
  root.append_attribute("nextobjectid").set_value(ir_map.next_object_id);

  append_properties(root, ir_map.context);

  for (const auto& ir_tileset: ir_map.tilesets) {
    append_tileset(root, dir, ir_tileset);
  }

  for (const auto& layer: ir_map.layers) {
    append_layer(root, ir_map, layer);
  }
}

}  // namespace

void save_map_as_tiled_xml(const Path& destination, const MapIR& ir_map)
{
  if (!ir_map.component_definitions.empty()) {
    spdlog::warn("Component data will be ignored when saving the map as XML!");
  }

  pugi::xml_document document;
  append_root(document, destination.parent_path(), ir_map);

  if (save_xml_to_file(document, destination).failed()) {
    spdlog::error("Could not save XML map file");
  }
}

}  // namespace tactile