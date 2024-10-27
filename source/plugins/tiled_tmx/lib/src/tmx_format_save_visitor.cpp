// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmx/tmx_format_save_visitor.hpp"

#include <filesystem>  // relative
#include <format>      // format
#include <sstream>     // stringstream
#include <stdexcept>   // invalid_argument
#include <utility>     // move

#include <cppcodec/base64_default_rfc4648.hpp>

#include "tactile/base/document/layer_view.hpp"
#include "tactile/base/document/map_view.hpp"
#include "tactile/base/document/meta_view.hpp"
#include "tactile/base/document/object_view.hpp"
#include "tactile/base/document/tile_view.hpp"
#include "tactile/base/document/tileset_view.hpp"
#include "tactile/base/io/compress/compression_format.hpp"
#include "tactile/base/numeric/literals.hpp"
#include "tactile/tiled_tmx/logging.hpp"
#include "tactile/tiled_tmx/tmx_common.hpp"

namespace tactile::tiled_tmx {
namespace {

void _append_property_attr(pugi::xml_node node, const Attribute& property)
{
  auto value_attribute = node.append_attribute("value");

  switch (property.get_type()) {
    case AttributeType::kStr: {
      value_attribute.set_value(property.as_string().c_str());
      break;
    }
    case AttributeType::kInt: {
      value_attribute.set_value(property.as_int());
      break;
    }
    case AttributeType::kInt2: {
      const auto& i2 = property.as_int2();

      const auto str = std::format("{};{}", i2.x(), i2.y());
      value_attribute.set_value(str.c_str());

      break;
    }
    case AttributeType::kInt3: {
      const auto& i3 = property.as_int3();

      const auto str = std::format("{};{};{}", i3.x(), i3.y(), i3.z());
      value_attribute.set_value(str.c_str());

      break;
    }
    case AttributeType::kInt4: {
      const auto& i4 = property.as_int4();

      const auto str = std::format("{};{};{};{}", i4.x(), i4.y(), i4.z(), i4.w());
      value_attribute.set_value(str.c_str());

      break;
    }
    case AttributeType::kFloat: {
      value_attribute.set_value(property.as_float());
      break;
    }
    case AttributeType::kFloat2: {
      const auto& f2 = property.as_float2();

      const auto str = std::format("{};{}", f2.x(), f2.y());
      value_attribute.set_value(str.c_str());

      break;
    }
    case AttributeType::kFloat3: {
      const auto& f3 = property.as_float3();

      const auto str = std::format("{};{};{}", f3.x(), f3.y(), f3.z());
      value_attribute.set_value(str.c_str());

      break;
    }
    case AttributeType::kFloat4: {
      const auto& f4 = property.as_float4();

      const auto str = std::format("{};{};{};{}", f4.x(), f4.y(), f4.z(), f4.w());
      value_attribute.set_value(str.c_str());

      break;
    }
    case AttributeType::kBool: {
      value_attribute.set_value(property.as_bool());
      break;
    }
    case AttributeType::kPath: {
      const auto path_str = property.as_path().string();
      value_attribute.set_value(path_str.c_str());
      break;
    }
    case AttributeType::kColor: {
      const auto argb_str = to_string(property.as_color(), ColorFormat::kArgb);
      value_attribute.set_value(argb_str.c_str());
      break;
    }
    case AttributeType::kObject: {
      value_attribute.set_value(property.as_object().value);
      break;
    }
    default: throw std::invalid_argument {"bad attribute type"};
  }
}

void _append_properties_node(pugi::xml_node node, const IMetaView& meta)
{
  const auto count = meta.property_count();
  if (count < 1) {
    return;
  }

  auto properties_node = node.append_child("properties");

  for (auto index = 0_uz; index < count; ++index) {
    const auto& [name, value] = meta.get_property(index);

    auto property_node = properties_node.append_child("property");
    property_node.append_attribute("name").set_value(name.c_str());

    // Properties with no type attribute are assumed to be string properties
    const auto type = value.get_type();
    if (type != AttributeType::kStr && !value.is_vector()) {
      const auto* prop_type_name = get_property_type_name(type);
      property_node.append_attribute("type").set_value(prop_type_name);
    }

    _append_property_attr(property_node, value);
  }
}

void _add_csv_tile_data(pugi::xml_node node, const ILayerView& layer)
{
  node.append_attribute("encoding").set_value("csv");

  std::stringstream stream {};

  const auto extent = layer.get_extent().value();
  for (auto row = 0_uz; row < extent.rows; ++row) {
    for (auto col = 0_uz; col < extent.cols; ++col) {
      const Index2D index {col, row};
      const auto tile_id = layer.get_tile(index).value();

      if (row != 0_uz || col != 0_uz) {
        stream << ',';
      }

      stream << tile_id;
    }
  }

  const auto csv_str = stream.str();
  node.text().set(csv_str.c_str());
}

[[nodiscard]]
auto _add_base64_tile_data(pugi::xml_node data_node,
                           const IRuntime& runtime,
                           const ILayerView& layer) -> std::expected<void, ErrorCode>
{
  data_node.append_attribute("encoding").set_value("base64");

  ByteStream tile_bytes {};

  const auto extent = layer.get_extent().value();
  tile_bytes.reserve(sizeof(TileID) * extent.rows * extent.cols);

  layer.write_tile_bytes(tile_bytes);

  if (const auto compress_format_id = layer.get_tile_compression()) {
    auto* compression_format = runtime.get_compression_format(*compress_format_id);

    if (!compression_format) {
      TACTILE_TILED_TMX_ERROR("No suitable compression plugin available");
      return std::unexpected {ErrorCode::kNotSupported};
    }

    if (auto compressed_tile_bytes = compression_format->compress(tile_bytes)) {
      tile_bytes = std::move(*compressed_tile_bytes);
    }
    else {
      TACTILE_TILED_TMX_ERROR("Could not compress tile data");
      return std::unexpected {compressed_tile_bytes.error()};
    }

    const char* compress_format_name = get_compression_format_name(*compress_format_id);
    data_node.append_attribute("compression").set_value(compress_format_name);
  }

  const auto encoded_tile_bytes = base64::encode(tile_bytes);
  data_node.text().set(encoded_tile_bytes.c_str());

  return {};
}

void _append_animation_node(pugi::xml_node tile_node, const ITileView& tile)
{
  auto animation_node = tile_node.append_child("animation");

  const auto frame_count = tile.animation_frame_count();
  for (auto frame_index = 0_uz; frame_index < frame_count; ++frame_index) {
    const auto& [frame_tile_index, frame_duration] = tile.get_animation_frame(frame_index);

    auto frame_node = animation_node.append_child("frame");
    frame_node.append_attribute("tileid").set_value(frame_tile_index);
    frame_node.append_attribute("duration").set_value(frame_duration.count());
  }
}

void _append_tileset_image_node(pugi::xml_node tileset_node,
                                const ITilesetView& tileset,
                                const SaveFormatWriteOptions& options)
{
  const auto& image_path = tileset.get_image_path();
  const auto image_size = tileset.get_image_size();

  const auto source = std::filesystem::relative(image_path, options.base_dir).string();

  auto image_node = tileset_node.append_child("image");
  image_node.append_attribute("source").set_value(source.c_str());
  image_node.append_attribute("width").set_value(image_size.x());
  image_node.append_attribute("height").set_value(image_size.y());
}

void _add_common_tileset_data(pugi::xml_node tileset_node,
                              const ITilesetView& tileset,
                              const SaveFormatWriteOptions& options)
{
  const auto& meta = tileset.get_meta();
  const auto tile_size = tileset.get_tile_size();

  tileset_node.append_attribute("name").set_value(meta.get_name().data());  // FIXME c_str
  tileset_node.append_attribute("tilewidth").set_value(tile_size.x());
  tileset_node.append_attribute("tileheight").set_value(tile_size.y());
  tileset_node.append_attribute("tilecount").set_value(tileset.tile_count());
  tileset_node.append_attribute("columns").set_value(tileset.column_count());

  _append_tileset_image_node(tileset_node, tileset, options);
  _append_properties_node(tileset_node, meta);
}

}  // namespace

TmxFormatSaveVisitor::TmxFormatSaveVisitor(IRuntime* runtime, SaveFormatWriteOptions options)
  : m_runtime {runtime},
    m_options {std::move(options)},
    m_map_document {},
    m_map_node {},
    m_layer_nodes {},
    m_tileset_documents {}
{}

auto TmxFormatSaveVisitor::visit(const IComponentView& component)
    -> std::expected<void, ErrorCode>
{
  return {};
}

auto TmxFormatSaveVisitor::visit(const IMapView& map) -> std::expected<void, ErrorCode>
{
  const auto layer_count = map.layer_count();
  m_layer_nodes.resize(layer_count);

  if (m_options.use_external_tilesets) {
    const auto tileset_count = map.tileset_count();
    m_tileset_documents.reserve(tileset_count);
  }

  m_map_node = m_map_document.append_child("map");

  const auto extent = map.get_extent();
  const auto tile_size = map.get_tile_size();
  m_map_node.append_attribute("version").set_value("1.7");
  m_map_node.append_attribute("tiledversion").set_value("1.9.0");
  m_map_node.append_attribute("orientation").set_value("orthogonal");
  m_map_node.append_attribute("renderorder").set_value("right-down");
  m_map_node.append_attribute("infinite").set_value(false);
  m_map_node.append_attribute("tilewidth").set_value(tile_size.x());
  m_map_node.append_attribute("tileheight").set_value(tile_size.y());
  m_map_node.append_attribute("width").set_value(extent.cols);
  m_map_node.append_attribute("height").set_value(extent.rows);
  m_map_node.append_attribute("nextlayerid").set_value(map.get_next_layer_id());
  m_map_node.append_attribute("nextobjectid").set_value(map.get_next_object_id());

  _append_properties_node(m_map_node, map.get_meta());

  return {};
}

auto TmxFormatSaveVisitor::visit(const ILayerView& layer) -> std::expected<void, ErrorCode>
{
  const auto layer_type = layer.get_type();
  const char* node_name = get_layer_type_name(layer_type);

  pugi::xml_node layer_node {};
  if (const auto* parent_layer = layer.get_parent_layer()) {
    auto parent_node = m_layer_nodes.at(parent_layer->get_global_index());
    layer_node = parent_node.append_child(node_name);
  }
  else {
    layer_node = m_map_node.append_child(node_name);
  }

  const auto& meta = layer.get_meta();
  layer_node.append_attribute("id").set_value(layer.get_id());
  layer_node.append_attribute("name").set_value(meta.get_name().data());

  if (const auto opacity = layer.get_opacity(); opacity != 1.0f) {
    layer_node.append_attribute("opacity").set_value(opacity);
  }

  if (!layer.is_visible()) {
    layer_node.append_attribute("visible").set_value(false);
  }

  switch (layer_type) {
    case LayerType::kTileLayer: {
      const auto extent = layer.get_extent().value();
      const auto tile_encoding = layer.get_tile_encoding();

      layer_node.append_attribute("width").set_value(extent.cols);
      layer_node.append_attribute("height").set_value(extent.rows);

      const auto data_node = layer_node.append_child("data");
      switch (tile_encoding) {
        case TileEncoding::kPlainText: {
          _add_csv_tile_data(data_node, layer);
          break;
        }
        case TileEncoding::kBase64: {
          const auto add_base64_tile_data_result =
              _add_base64_tile_data(data_node, *m_runtime, layer);

          if (!add_base64_tile_data_result.has_value()) {
            return std::unexpected {add_base64_tile_data_result.error()};
          }

          break;
        }
        default: throw std::invalid_argument {"bad tile encoding"};
      }

      break;
    }
    case LayerType::kObjectLayer: /* Do nothing */ break;
    case LayerType::kGroupLayer:  /* Do nothing */ break;
    default:                      throw std::invalid_argument {"bad layer type"};
  }

  _append_properties_node(layer_node, layer.get_meta());

  const auto layer_index = layer.get_global_index();
  m_layer_nodes.at(layer_index) = std::move(layer_node);

  return {};
}

auto TmxFormatSaveVisitor::visit(const IObjectView& object) -> std::expected<void, ErrorCode>
{
  pugi::xml_node parent_node {};
  if (const auto* parent_layer = object.get_parent_layer()) {
    parent_node = m_layer_nodes.at(parent_layer->get_global_index());
  }
  else if (const auto* parent_tile = object.get_parent_tile()) {
    const auto tile_node =
        _get_tile_node(parent_tile->get_parent_tileset(), parent_tile->get_index());
    parent_node = tile_node.child("objectgroup");
  }
  else {
    return std::unexpected {ErrorCode::kBadState};
  }

  const auto& meta = object.get_meta();

  const auto position = object.get_position();
  const auto size = object.get_size();
  const auto type = object.get_type();

  auto object_node = parent_node.append_child("object");
  object_node.append_attribute("id").set_value(object.get_id());

  if (const auto name = meta.get_name(); !name.empty()) {
    object_node.append_attribute("name").set_value(name.data());  // FIXME c_str
  }

  if (const auto tag = object.get_tag(); !tag.empty()) {
    object_node.append_attribute("type").set_value(tag.data());  // FIXME c_str
  }

  if (position.x() != 0.0f) {
    object_node.append_attribute("x").set_value(position.x());
  }

  if (position.y() != 0.0f) {
    object_node.append_attribute("y").set_value(position.y());
  }

  if (size.x() != 0.0f) {
    object_node.append_attribute("width").set_value(size.x());
  }

  if (size.y() != 0.0f) {
    object_node.append_attribute("height").set_value(size.y());
  }

  if (!object.is_visible()) {
    object_node.append_attribute("visible").set_value(false);
  }

  // Objects are rectangles if no type tag is present
  if (type == ObjectType::kPoint) {
    object_node.append_child("point");
  }
  else if (type == ObjectType::kEllipse) {
    object_node.append_child("ellipse");
  }

  _append_properties_node(object_node, meta);

  return {};
}

auto TmxFormatSaveVisitor::visit(const ITilesetView& tileset) -> std::expected<void, ErrorCode>
{
  const auto first_tile_id = tileset.get_first_tile_id();

  auto embedded_tileset_node = m_map_node.append_child("tileset");
  embedded_tileset_node.append_attribute("firstgid").set_value(first_tile_id);

  if (m_options.use_external_tilesets) {
    auto source = std::format("{}.tsx", tileset.get_filename());
    embedded_tileset_node.append_attribute("source").set_value(source.c_str());

    auto& external_tileset_document = m_tileset_documents.emplace_back();
    external_tileset_document.source = std::move(source);

    const auto external_tileset_node =
        external_tileset_document.document.append_child("tileset");
    _add_common_tileset_data(external_tileset_node, tileset, m_options);

    m_tileset_nodes[first_tile_id] = external_tileset_node;
  }
  else {
    _add_common_tileset_data(embedded_tileset_node, tileset, m_options);
    m_tileset_nodes[first_tile_id] = embedded_tileset_node;
  }

  return {};
}

auto TmxFormatSaveVisitor::visit(const ITileView& tile) -> std::expected<void, ErrorCode>
{
  const auto& tileset = tile.get_parent_tileset();
  auto tileset_node = m_tileset_nodes.at(tileset.get_first_tile_id());

  auto tile_node = tileset_node.append_child("tile");
  tile_node.append_attribute("id").set_value(tile.get_index());

  if (tile.animation_frame_count() > 0) {
    _append_animation_node(tile_node, tile);
  }

  if (tile.object_count() > 0) {
    tile_node.append_child("objectgroup");
  }

  _append_properties_node(tile_node, tile.get_meta());

  return {};
}

auto TmxFormatSaveVisitor::get_map_xml_document() const -> const pugi::xml_document&
{
  return m_map_document;
}

auto TmxFormatSaveVisitor::get_tileset_xml_documents() const
    -> const std::vector<TmxTilesetDocument>&
{
  return m_tileset_documents;
}

auto TmxFormatSaveVisitor::_get_tile_node(const ITilesetView& tileset,
                                          const TileIndex tile_index) -> pugi::xml_node
{
  const auto tileset_iter = m_tileset_nodes.find(tileset.get_first_tile_id());

  if (tileset_iter != m_tileset_nodes.end()) {
    const auto& tileset_node = tileset_iter->second;

    for (const auto& tile_node : tileset_node.children("tile")) {
      const auto id = tile_node.attribute("id").as_int(-1);
      if (id == tile_index) {
        return tile_node;
      }
    }
  }

  throw std::invalid_argument {"no such tile node"};
}

}  // namespace tactile::tiled_tmx
