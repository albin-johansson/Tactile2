// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmj/tmj_format_save_visitor.hpp"

#include <cstddef>    // size_t
#include <format>     // format
#include <stdexcept>  // runtime_error
#include <utility>    // move

#include <cppcodec/base64_default_rfc4648.hpp>

#include "tactile/base/document/layer_view.hpp"
#include "tactile/base/document/map_view.hpp"
#include "tactile/base/document/meta_view.hpp"
#include "tactile/base/document/object_view.hpp"
#include "tactile/base/document/tile_view.hpp"
#include "tactile/base/document/tileset_view.hpp"
#include "tactile/base/io/compress/compression_format.hpp"
#include "tactile/base/io/tile_io.hpp"
#include "tactile/base/numeric/literals.hpp"
#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/base/platform/filesystem.hpp"
#include "tactile/tiled_tmj/logging.hpp"

namespace tactile::tiled_tmj {
namespace {

[[nodiscard]]
auto _emit_property(const IMetaView& meta, const std::size_t property_index) -> JSON
{
  const auto& [property_name, property_value] = meta.get_property(property_index);

  auto property_json = JSON::object();
  property_json["name"] = property_name;

  switch (property_value.get_type()) {
    case AttributeType::kStr: {
      property_json["type"] = "string";
      property_json["value"] = property_value.as_string();
      break;
    }
    case AttributeType::kInt: {
      property_json["type"] = "int";
      property_json["value"] = property_value.as_int();
      break;
    }
    case AttributeType::kInt2: {
      const auto& i2 = property_value.as_int2();

      property_json["type"] = "string";
      property_json["value"] = std::format("{};{}", i2.x(), i2.y());

      break;
    }
    case AttributeType::kInt3: {
      const auto& i3 = property_value.as_int3();

      property_json["type"] = "string";
      property_json["value"] = std::format("{};{};{}", i3.x(), i3.y(), i3.z());

      break;
    }
    case AttributeType::kInt4: {
      const auto& i4 = property_value.as_int4();

      property_json["type"] = "string";
      property_json["value"] = std::format("{};{};{};{}", i4.x(), i4.y(), i4.z(), i4.w());

      break;
    }
    case AttributeType::kFloat: {
      property_json["type"] = "float";
      property_json["value"] = property_value.as_float();
      break;
    }
    case AttributeType::kFloat2: {
      const auto& f2 = property_value.as_float2();

      property_json["type"] = "string";
      property_json["value"] = std::format("{};{}", f2.x(), f2.y());

      break;
    }
    case AttributeType::kFloat3: {
      const auto& f3 = property_value.as_float3();

      property_json["type"] = "string";
      property_json["value"] = std::format("{};{};{}", f3.x(), f3.y(), f3.z());

      break;
    }
    case AttributeType::kFloat4: {
      const auto& f4 = property_value.as_float4();

      property_json["type"] = "string";
      property_json["value"] = std::format("{};{};{};{}", f4.x(), f4.y(), f4.z(), f4.w());

      break;
    }
    case AttributeType::kBool: {
      property_json["type"] = "bool";
      property_json["value"] = property_value.as_bool();
      break;
    }
    case AttributeType::kPath: {
      property_json["type"] = "file";
      property_json["value"] = property_value.as_path().string();
      break;
    }
    case AttributeType::kColor: {
      const auto& color = property_value.as_color();

      property_json["type"] = "color";
      property_json["value"] = std::format("#{:02X}{:02X}{:02X}{:02X}",
                                           +color.alpha,
                                           +color.red,
                                           +color.green,
                                           +color.blue);

      break;
    }
    case AttributeType::kObject: {
      property_json["type"] = "object";
      property_json["value"] = property_value.as_object().value;
      break;
    }
  }

  return property_json;
}

void _emit_metadata(const IMetaView& meta, JSON& meta_json)
{
  const auto property_count = meta.property_count();
  if (property_count == 0) {
    return;
  }

  auto property_array = JSON::array();
  property_array.get_ref<JSON::array_t&>().reserve(property_count);

  for (std::size_t index = 0; index < property_count; ++index) {
    property_array.push_back(_emit_property(meta, index));
  }

  meta_json["properties"] = std::move(property_array);
}

void _save_common_tileset_attributes(const ITilesetView& tileset,
                                     JSON& tileset_json,
                                     const SaveFormatWriteOptions& options)
{
  const auto tile_size = tileset.get_tile_size();
  const auto image_size = tileset.get_image_size();

  const auto relative_image_path =
      std::filesystem::relative(tileset.get_image_path(), options.base_dir);

  tileset_json["name"] = tileset.get_meta().get_name();
  tileset_json["columns"] = tileset.column_count();

  tileset_json["tilewidth"] = tile_size.x();
  tileset_json["tileheight"] = tile_size.y();
  tileset_json["tilecount"] = tileset.tile_count();

  tileset_json["image"] = normalize_path_separators(relative_image_path);
  tileset_json["imagewidth"] = image_size.x();
  tileset_json["imageheight"] = image_size.y();

  tileset_json["margin"] = 0;
  tileset_json["spacing"] = 0;

  const auto tile_definition_count = tileset.tile_definition_count();
  if (tile_definition_count > 0) {
    auto tile_array = JSON::array();
    tile_array.get_ref<JSON::array_t&>().reserve(tile_definition_count);
    tileset_json["tiles"] = std::move(tile_array);
  }

  _emit_metadata(tileset.get_meta(), tileset_json);
}

[[nodiscard]]
auto _emit_tile_layer(const IRuntime& runtime,
                      const ILayerView& layer,
                      JSON& layer_json,
                      ByteStream& tile_bytes) -> std::expected<void, ErrorCode>
{
  const auto tile_encoding = layer.get_tile_encoding();
  const auto tile_compression = layer.get_tile_compression();
  const auto extent = layer.get_extent().value();

  layer_json["type"] = "tilelayer";
  layer_json["width"] = extent.cols;
  layer_json["height"] = extent.rows;

  if (tile_encoding == TileEncoding::kBase64) {
    layer_json["encoding"] = "base64";
  }

  if (tile_compression == CompressionFormatId::kZlib) {
    layer_json["compression"] = "zlib";
  }
  else if (tile_compression == CompressionFormatId::kZstd) {
    layer_json["compression"] = "zstd";
  }

  if (tile_encoding == TileEncoding::kBase64) {
    tile_bytes.clear();
    layer.write_tile_bytes(tile_bytes);

    if (tile_compression.has_value()) {
      const auto* compression_format = runtime.get_compression_format(*tile_compression);
      if (!compression_format) {
        TACTILE_TILED_TMJ_ERROR("Could not find suitable compression format");
        return std::unexpected {ErrorCode::kNotSupported};
      }

      auto compressed_tile_bytes = compression_format->compress(tile_bytes);
      if (!compressed_tile_bytes.has_value()) {
        return std::unexpected {compressed_tile_bytes.error()};
      }

      tile_bytes = std::move(*compressed_tile_bytes);
    }

    auto encoded_bytes = base64::encode(tile_bytes);
    layer_json["data"] = std::move(encoded_bytes);
  }
  else {
    auto tile_array = JSON::array();
    tile_array.get_ref<JSON::array_t&>().reserve(
        saturate_cast<std::size_t>(extent.rows * extent.cols));

    for (Extent2D::value_type row = 0; row < extent.rows; ++row) {
      for (Extent2D::value_type col = 0; col < extent.cols; ++col) {
        const Index2D index {.x = col, .y = row};
        const auto tile_id = layer.get_tile(index).value_or(kEmptyTile);
        tile_array.push_back(tile_id);
      }
    }

    layer_json["data"] = std::move(tile_array);
  }

  return {};
}

void _emit_object_layer(const ILayerView& layer, JSON& layer_json)
{
  auto object_array = JSON::array();
  object_array.get_ref<JSON::array_t&>().reserve(layer.object_count());

  layer_json["type"] = "objectgroup";
  layer_json["objects"] = std::move(object_array);
}

void _emit_group_layer(const ILayerView& layer, JSON& layer_json)
{
  auto layer_array = JSON::array();
  layer_array.get_ref<JSON::array_t&>().reserve(layer.layer_count());

  layer_json["type"] = "group";
  layer_json["layers"] = std::move(layer_array);
}

}  // namespace

TmjFormatSaveVisitor::TmjFormatSaveVisitor(IRuntime* runtime, SaveFormatWriteOptions options)
  : m_runtime {runtime},
    m_options {std::move(options)}
{}

auto TmjFormatSaveVisitor::visit(const IMapView& map) -> std::expected<void, ErrorCode>
{
  if (map.component_count() > 0) {
    TACTILE_TILED_TMJ_WARN("Components are ignored in Tiled TMJ save files");
  }

  const auto extent = map.get_extent();
  const auto tile_size = map.get_tile_size();

  m_map_node = JSON::object();
  m_map_node["type"] = "map";
  m_map_node["tiledversion"] = "1.9.0";
  m_map_node["version"] = "1.7";
  m_map_node["renderorder"] = "right-down";
  m_map_node["orientation"] = "orthogonal";
  m_map_node["infinite"] = false;
  m_map_node["width"] = extent.cols;
  m_map_node["height"] = extent.rows;
  m_map_node["tilewidth"] = tile_size.x();
  m_map_node["tileheight"] = tile_size.y();
  m_map_node["nextlayerid"] = map.get_next_layer_id();
  m_map_node["nextobjectid"] = map.get_next_object_id();
  m_map_node["compressionlevel"] = map.get_compression_level().value_or(-1);

  _emit_metadata(map.get_meta(), m_map_node);

  auto tileset_json_array = JSON::array();
  tileset_json_array.get_ref<JSON::array_t&>().reserve(map.tileset_count());
  m_map_node["tilesets"] = std::move(tileset_json_array);

  auto layer_json_array = JSON::array();
  layer_json_array.get_ref<JSON::array_t&>().reserve(map.layer_count());
  m_map_node["layers"] = std::move(layer_json_array);

  if (m_options.use_external_tilesets) {
    m_external_tileset_nodes.reserve(map.tileset_count());
  }

  return {};
}

auto TmjFormatSaveVisitor::visit(const ITilesetView& tileset) -> std::expected<void, ErrorCode>
{
  auto embedded_tileset_json = JSON::object();
  embedded_tileset_json["firstgid"] = tileset.get_first_tile_id();

  if (m_options.use_external_tilesets) {
    const auto source_path = std::format("{}.tsj", tileset.get_filename());
    embedded_tileset_json["source"] = source_path;

    auto external_tileset_json = JSON::object();
    _save_common_tileset_attributes(tileset, external_tileset_json, m_options);

    TmjFormatExternalTilesetData external_tileset {};
    external_tileset.path = m_options.base_dir / source_path;
    external_tileset.json = std::move(external_tileset_json);
    m_external_tileset_nodes[tileset.get_first_tile_id()] = std::move(external_tileset);
  }
  else {
    _save_common_tileset_attributes(tileset, embedded_tileset_json, m_options);
  }

  m_map_node.at("tilesets").push_back(std::move(embedded_tileset_json));

  return {};
}

auto TmjFormatSaveVisitor::visit(const ITileView& tile) -> std::expected<void, ErrorCode>
{
  auto tile_json = JSON::object();
  tile_json["id"] = tile.get_index();

  if (const auto frame_count = tile.animation_frame_count(); frame_count > 0) {
    auto animation_json = JSON::array();
    animation_json.get_ref<JSON::array_t&>().reserve(frame_count);

    for (auto frame_index = 0_uz; frame_index < frame_count; ++frame_index) {
      auto frame_json = JSON::object();

      const auto [frame_tile, frame_duration] = tile.get_animation_frame(frame_index);
      frame_json["tileid"] = frame_tile;
      frame_json["duration"] = frame_duration.count();

      animation_json.push_back(std::move(frame_json));
    }

    tile_json["animation"] = std::move(animation_json);
  }

  if (const auto object_count = tile.object_count(); object_count > 0) {
    auto object_array = JSON::array();
    object_array.get_ref<JSON::array_t&>().reserve(object_count);

    auto dummy_layer = JSON::object();

    // Normal layers feature an "id" attribute, but Tiled seems to be fine without it here.
    dummy_layer["draworder"] = "index";
    dummy_layer["name"] = "";
    dummy_layer["opacity"] = 1;
    dummy_layer["type"] = "objectgroup";
    dummy_layer["visible"] = true;
    dummy_layer["x"] = 0;
    dummy_layer["y"] = 0;
    dummy_layer["objects"] = std::move(object_array);

    tile_json["objectgroup"] = std::move(dummy_layer);
  }

  _emit_metadata(tile.get_meta(), tile_json);

  const auto& tileset = tile.get_parent_tileset();
  auto& tileset_json = _get_tileset_json(tileset);
  tileset_json.at("tiles").push_back(std::move(tile_json));

  return {};
}

auto TmjFormatSaveVisitor::visit(const ILayerView& layer) -> std::expected<void, ErrorCode>
{
  auto layer_json = JSON::object();
  layer_json["name"] = layer.get_meta().get_name();
  layer_json["id"] = layer.get_id();
  layer_json["opacity"] = layer.get_opacity();
  layer_json["visible"] = layer.is_visible();
  layer_json["x"] = 0;
  layer_json["y"] = 0;

  switch (layer.get_type()) {
    case LayerType::kTileLayer: {
      const auto emit_tile_layer_result =
          _emit_tile_layer(*m_runtime, layer, layer_json, m_tile_byte_cache);

      if (!emit_tile_layer_result) {
        return std::unexpected {emit_tile_layer_result.error()};
      }

      break;
    }
    case LayerType::kObjectLayer: {
      _emit_object_layer(layer, layer_json);
      break;
    }
    case LayerType::kGroupLayer: {
      _emit_group_layer(layer, layer_json);
      break;
    }
  }

  _emit_metadata(layer.get_meta(), layer_json);

  if (const auto* parent_layer = layer.get_parent_layer()) {
    auto& parent_layer_json = _get_layer_json(*parent_layer);
    parent_layer_json.at("layers").push_back(std::move(layer_json));
  }
  else {
    m_map_node.at("layers").push_back(std::move(layer_json));
  }

  return {};
}

auto TmjFormatSaveVisitor::visit(const IObjectView& object) -> std::expected<void, ErrorCode>
{
  const auto object_position = object.get_position();
  const auto object_size = object.get_size();
  const auto object_type = object.get_type();

  auto object_json = JSON::object();
  object_json["name"] = object.get_meta().get_name();
  object_json["id"] = object.get_id();
  object_json["x"] = object_position.x();
  object_json["y"] = object_position.y();
  object_json["width"] = object_size.x();
  object_json["height"] = object_size.y();
  object_json["visible"] = object.is_visible();
  object_json["type"] = object.get_tag();
  object_json["rotation"] = 0;

  if (object_type == ObjectType::kPoint) {
    object_json["point"] = true;
  }
  else if (object_type == ObjectType::kEllipse) {
    object_json["ellipse"] = true;
  }

  _emit_metadata(object.get_meta(), object_json);

  if (const auto* parent_layer = object.get_parent_layer()) {
    auto& parent_layer_json = _get_layer_json(*parent_layer);
    parent_layer_json.at("objects").push_back(std::move(object_json));
  }
  else if (const auto* parent_tile = object.get_parent_tile()) {
    auto& tile_json = _get_tile_json(*parent_tile);
    tile_json.at("objectgroup").at("objects").push_back(std::move(object_json));
  }
  else {
    TACTILE_TILED_TMJ_ERROR("Object {} has no parent layer or tile", object.get_id());
    return std::unexpected {ErrorCode::kBadState};
  }

  return {};
}

auto TmjFormatSaveVisitor::visit(const IComponentView&) -> std::expected<void, ErrorCode>
{
  return {};
}

auto TmjFormatSaveVisitor::get_map_json() const -> const JSON&
{
  return m_map_node;
}

auto TmjFormatSaveVisitor::get_external_tilesets() const
    -> const std::unordered_map<TileID, TmjFormatExternalTilesetData>&
{
  return m_external_tileset_nodes;
}

auto TmjFormatSaveVisitor::_get_tile_json(const ITileView& tile) -> JSON&
{
  const auto& tileset = tile.get_parent_tileset();
  auto& tileset_json = _get_tileset_json(tileset);

  if (auto* tile_json = _find_tile_json(tileset_json, tile.get_index())) {
    return *tile_json;
  }

  throw std::runtime_error {"no such tile node"};
}

auto TmjFormatSaveVisitor::_get_tileset_json(const ITilesetView& tileset) -> JSON&
{
  if (auto* tileset_json = _find_tileset_json(tileset.get_first_tile_id())) {
    return *tileset_json;
  }

  throw std::runtime_error {"no such tileset node"};
}

auto TmjFormatSaveVisitor::_get_layer_json(const ILayerView& layer) -> JSON&
{
  if (auto* layer_json = _find_layer_json(m_map_node, layer.get_id())) {
    return *layer_json;
  }

  throw std::runtime_error {"no such layer node"};
}

auto TmjFormatSaveVisitor::_find_layer_json(JSON& root_node, const LayerID id) -> JSON*
{
  if (!root_node.contains("layers")) {
    return nullptr;
  }

  for (const auto& [_, layer_node] : root_node.at("layers").items()) {
    if (layer_node.at("id") == id) {
      return &layer_node;
    }

    if (auto* found_node = _find_layer_json(layer_node, id)) {
      return found_node;
    }
  }

  return nullptr;
}

auto TmjFormatSaveVisitor::_find_tileset_json(const TileID first_tile_id) -> JSON*
{
  if (m_options.use_external_tilesets) {
    const auto external_tileset_iter = m_external_tileset_nodes.find(first_tile_id);
    if (external_tileset_iter != m_external_tileset_nodes.end()) {
      return &external_tileset_iter->second.json;
    }
  }
  else {
    for (const auto& [_, tileset_node] : m_map_node.at("tilesets").items()) {
      if (tileset_node.at("firstgid") == first_tile_id) {
        return &tileset_node;
      }
    }
  }

  return nullptr;
}

auto TmjFormatSaveVisitor::_find_tile_json(JSON& tileset_json, const TileIndex tile_index)
    -> JSON*
{
  for (const auto& [_, tile_json] : tileset_json.at("tiles").items()) {
    if (tile_json.at("id") == tile_index) {
      return &tile_json;
    }
  }

  return nullptr;
}

}  // namespace tactile::tiled_tmj
