// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmj/tmj_format_parser.hpp"

#include <iterator>   // distance
#include <optional>   // optional, nullopt
#include <stdexcept>  // invalid_argument
#include <string>     // string
#include <utility>    // move, cmp_not_equal

#include <cppcodec/base64_default_rfc4648.hpp>
#include <nlohmann/json.hpp>

#include "tactile/base/io/compress/compression_format.hpp"
#include "tactile/base/io/tile_io.hpp"
#include "tactile/base/meta/color.hpp"
#include "tactile/base/numeric/literals.hpp"
#include "tactile/base/util/tile_matrix.hpp"
#include "tactile/runtime/logging.hpp"
#include "tactile/tiled_tmj/tmj_common.hpp"

namespace tactile::tiled_tmj {
namespace {

[[nodiscard]]
auto _read_object_layer(const JSON& layer_json, std::vector<ir::Object>& objects)
    -> std::expected<void, ErrorCode>;

[[nodiscard]]
auto _read_layers(const IRuntime& runtime,
                  const JSON& root_json,
                  std::vector<ir::Layer>& layers,
                  ir::TileFormat& tile_format) -> std::expected<void, ErrorCode>;

[[nodiscard]]
auto _read_property_value(const JSON& property_json, const AttributeType type)
    -> std::expected<Attribute, ErrorCode>
{
  switch (type) {
    case AttributeType::kStr: {
      if (auto read_value = read_attr<Attribute::string_type>(property_json, "value")) {
        return Attribute {std::move(*read_value)};
      }
      break;
    }
    case AttributeType::kInt: {
      if (const auto read_value = read_attr<Attribute::int_type>(property_json, "value")) {
        return Attribute {*read_value};
      }
      break;
    }
    case AttributeType::kFloat: {
      if (const auto read_value = read_attr<Attribute::float_type>(property_json, "value")) {
        return Attribute {*read_value};
      }
      break;
    }
    case AttributeType::kBool: {
      if (const auto read_value = read_attr<bool>(property_json, "value")) {
        return Attribute {*read_value};
      }
      break;
    }
    case AttributeType::kPath: {
      if (const auto read_value = read_attr<Attribute::string_type>(property_json, "value")) {
        return Attribute {Attribute::path_type {*read_value}};
      }
      break;
    }
    case AttributeType::kColor: {
      if (const auto read_value = read_attr<Attribute::string_type>(property_json, "value")) {
        const auto color = read_value->size() == 9 ? parse_color_argb(*read_value)
                                                   : parse_color_rgb(*read_value);
        if (color.has_value()) {
          return Attribute {*color};
        }
      }
      break;
    }
    case AttributeType::kObject: {
      if (const auto read_value =
              read_attr<Attribute::objref_type::value_type>(property_json, "value")) {
        return Attribute {Attribute::objref_type {*read_value}};
      }
      break;
    }
    case AttributeType::kInt2:
    case AttributeType::kInt3:
    case AttributeType::kInt4:
    case AttributeType::kFloat2:
    case AttributeType::kFloat3:
    case AttributeType::kFloat4:
    default:                     break;
  }

  throw std::invalid_argument {"bad attribute type"};
}

[[nodiscard]]
auto _read_named_attribute(const nlohmann::json& property_json)
    -> std::expected<ir::NamedAttribute, ErrorCode>
{
  ir::NamedAttribute named_attribute {};
  return read_attr_to(property_json, "name", named_attribute.name)
      .and_then([&] { return read_attr<std::string>(property_json, "type"); })
      .and_then([&](const std::string& type_name) { return read_property_type(type_name); })
      .and_then(
          [&](const AttributeType type) { return _read_property_value(property_json, type); })
      .transform([&](Attribute&& value) {
        named_attribute.value = std::move(value);
        return std::move(named_attribute);
      });
}

[[nodiscard]]
auto _read_metadata(const nlohmann::json& root_json, ir::Metadata& meta)
    -> std::expected<void, ErrorCode>
{
  return read_array<ir::NamedAttribute>(root_json, "properties", &_read_named_attribute)
      .and_then([&](std::vector<ir::NamedAttribute>&& properties) {
        meta.properties = std::move(properties);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_animation_frame(const JSON& frame_json)
    -> std::expected<ir::AnimationFrame, ErrorCode>
{
  ir::AnimationFrame frame {};
  std::chrono::milliseconds::rep duration {};

  return read_attr_to(frame_json, "tileid", frame.tile_index)
      .and_then([&] { return read_attr_to(frame_json, "duration", duration); })
      .transform([&] {
        frame.duration = std::chrono::milliseconds {duration};
        return std::move(frame);
      });
}

[[nodiscard]]
auto _read_tileset_tile(const JSON& tile_json) -> std::expected<ir::Tile, ErrorCode>
{
  ir::Tile tile {};
  return read_attr_to(tile_json, "id", tile.index)
      .and_then([&] {
        return read_array<ir::AnimationFrame>(tile_json, "animation", &_read_animation_frame);
      })
      .and_then([&](std::vector<ir::AnimationFrame>&& animation) {
        tile.animation = std::move(animation);
        return std::expected<void, ErrorCode> {};
      })
      .and_then([&] { return _read_metadata(tile_json, tile.meta); })
      .and_then([&]() -> std::expected<void, ErrorCode> {
        const auto layer_json = tile_json.find("objectgroup");

        if (layer_json == tile_json.end()) {
          return {};
        }

        return _read_object_layer(*layer_json, tile.objects);
      })
      .transform([&] { return std::move(tile); });
}

[[nodiscard]]
auto _read_tileset_tiles(const JSON& tileset_json, ir::Tileset& tileset)
    -> std::expected<void, ErrorCode>
{
  return read_array<ir::Tile>(tileset_json, "tiles", &_read_tileset_tile)
      .and_then([&](std::vector<ir::Tile>&& tiles) {
        tileset.tiles = std::move(tiles);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_common_tileset_attributes(const JSON& tileset_json, ir::Tileset& tileset)
    -> std::expected<void, ErrorCode>
{
  return _read_metadata(tileset_json, tileset.meta)
      .and_then([&] { return read_attr_to(tileset_json, "name", tileset.meta.name); })
      .and_then([&] { return read_attr_to(tileset_json, "tilewidth", tileset.tile_size[0]); })
      .and_then([&] { return read_attr_to(tileset_json, "tileheight", tileset.tile_size[1]); })
      .and_then([&] { return read_attr_to(tileset_json, "tilecount", tileset.tile_count); })
      .and_then([&] { return read_attr_to(tileset_json, "columns", tileset.column_count); })
      .and_then(
          [&] { return read_attr_to(tileset_json, "imagewidth", tileset.image_size[0]); })
      .and_then(
          [&] { return read_attr_to(tileset_json, "imageheight", tileset.image_size[1]); })
      .and_then([&] { return read_attr<std::string>(tileset_json, "image"); })
      .and_then([&](const std::string& image_path) {
        tileset.image_path = image_path;
        return std::expected<void, ErrorCode> {};
      })
      .and_then([&] { return _read_tileset_tiles(tileset_json, tileset); });
}

[[nodiscard]]
auto _read_embedded_tileset(const JSON& tileset_json) -> std::expected<ir::Tileset, ErrorCode>
{
  ir::Tileset tileset {};
  tileset.is_embedded = true;

  return _read_common_tileset_attributes(tileset_json, tileset).transform([&] {
    return std::move(tileset);
  });
}

[[nodiscard]]
auto _read_external_tileset(const std::filesystem::path& path)
    -> std::expected<ir::Tileset, ErrorCode>
{
  ir::Tileset tileset {};
  tileset.is_embedded = false;

  return read_json_document(path)
      .and_then([&](const JSON& tileset_json) {
        return _read_common_tileset_attributes(tileset_json, tileset);
      })
      .transform([&] { return std::move(tileset); });
}

[[nodiscard]]
auto _read_tileset_ref(const JSON& tileset_ref_json, const SaveFormatReadOptions& options)
    -> std::expected<ir::TilesetRef, ErrorCode>
{
  ir::TilesetRef tileset_ref {};
  return read_attr_to(tileset_ref_json, "firstgid", tileset_ref.first_tile_id)
      .and_then([&] {
        const auto source = read_attr<std::string>(tileset_ref_json, "source");
        return source.has_value() ? _read_external_tileset(options.base_dir / *source)
                                  : _read_embedded_tileset(tileset_ref_json);
      })
      .transform([&](ir::Tileset&& tileset) {
        tileset_ref.tileset = std::move(tileset);
        return std::move(tileset_ref);
      });
}

[[nodiscard]]
auto _read_tilesets(const JSON& map_json, const SaveFormatReadOptions& options, ir::Map& map)
    -> std::expected<void, ErrorCode>
{
  const auto tileset_ref_parser = [&](const JSON& tileset_ref_json) {
    return _read_tileset_ref(tileset_ref_json, options);
  };

  return read_array<ir::TilesetRef>(map_json, "tilesets", tileset_ref_parser)
      .and_then([&](std::vector<ir::TilesetRef>&& tileset_refs) {
        map.tilesets = std::move(tileset_refs);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_base64_tile_data(const IRuntime& runtime,
                            const JSON& layer_json,
                            const Extent2D& extent,
                            const ir::TileFormat& tile_format)
    -> std::expected<TileMatrix, ErrorCode>
{
  const auto& encoded_tile_data = layer_json.at("data").get_ref<const JSON::string_t&>();
  auto decoded_bytes = base64::decode(encoded_tile_data);

  if (tile_format.compression.has_value()) {
    const auto* compression_format = runtime.get_compression_format(*tile_format.compression);
    if (!compression_format) {
      runtime::log(LogLevel::kError, "No suitable compression plugin available");
      return std::unexpected {ErrorCode::kNotSupported};
    }

    auto decompressed_bytes = compression_format->decompress(decoded_bytes);
    if (!decompressed_bytes.has_value()) {
      return std::unexpected {decompressed_bytes.error()};
    }

    decoded_bytes = std::move(*decompressed_bytes);
  }

  auto tile_matrix = parse_raw_tile_matrix(decoded_bytes, extent, TileIdFormat::kTiled);

  if (!tile_matrix.has_value()) {
    runtime::log(LogLevel::kError, "Could not parse raw tile matrix");
    return std::unexpected {ErrorCode::kParseError};
  }

  return std::move(*tile_matrix);
}

[[nodiscard]]
auto _read_plain_text_tile_data(const JSON& layer_json, const Extent2D& extent)
    -> std::expected<TileMatrix, ErrorCode>
{
  auto tile_matrix = make_tile_matrix(extent);

  const auto& data_json = layer_json.at("data");

  const auto expected_tile_count = extent.rows * extent.cols;
  const auto real_tile_count = std::ranges::distance(data_json.items());

  if (std::cmp_not_equal(expected_tile_count, real_tile_count)) {
    runtime::log(LogLevel::kError,
                 "Invalid tile count in layer (expected {} but got {})",
                 expected_tile_count,
                 real_tile_count);
    return std::unexpected {ErrorCode::kParseError};
  }

  Index2D::value_type tile_index = 0;
  for (const auto& [_, tile_json] : data_json.items()) {
    const auto position = Index2D::from_1d(tile_index, extent.cols);
    tile_json.get_to(tile_matrix[position.y][position.x]);

    ++tile_index;
  }

  return tile_matrix;
}

[[nodiscard]]
auto _read_tile_layer(const IRuntime& runtime,
                      const JSON& layer_json,
                      ir::Layer& layer,
                      ir::TileFormat& tile_format) -> std::expected<void, ErrorCode>
{
  return read_attr_to(layer_json, "width", layer.extent.cols)
      .and_then([&] { return read_attr_to(layer_json, "height", layer.extent.rows); })
      .transform(
          [&] { return read_attr<std::string>(layer_json, "encoding").value_or("csv"); })
      .and_then([&](const std::string& encoding) { return read_tile_encoding(encoding); })
      .transform([&](const TileEncoding encoding) -> std::string {
        tile_format.encoding = encoding;
        return read_attr<std::string>(layer_json, "compression").value_or("");
      })
      .and_then(
          [&](const std::string& compression) { return read_compression_format(compression); })
      .and_then([&](const std::optional<CompressionFormatId>& compression) {
        tile_format.compression = compression;

        switch (tile_format.encoding) {
          case TileEncoding::kPlainText: {
            return _read_plain_text_tile_data(layer_json, layer.extent);
          }
          case TileEncoding::kBase64: {
            return _read_base64_tile_data(runtime, layer_json, layer.extent, tile_format);
          }
          default: throw std::invalid_argument {"bad tile encoding"};
        }
      })
      .and_then([&](TileMatrix&& tile_matrix) {
        layer.tiles = std::move(tile_matrix);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_object(const JSON& object_json) -> std::expected<ir::Object, ErrorCode>
{
  ir::Object object {};
  object.tag = read_attr<std::string>(object_json, "type").value_or("");

  if (read_attr<bool>(object_json, "point").value_or(false)) {
    object.type = ObjectType::kPoint;
  }
  else if (read_attr<bool>(object_json, "ellipse").value_or(false)) {
    object.type = ObjectType::kEllipse;
  }
  else {
    object.type = ObjectType::kRect;
  }

  return read_attr_to(object_json, "id", object.id)
      .and_then([&] { return read_attr_to(object_json, "name", object.meta.name); })
      .and_then([&] { return read_attr_to(object_json, "x", object.position[0]); })
      .and_then([&] { return read_attr_to(object_json, "y", object.position[1]); })
      .and_then([&] { return read_attr_to(object_json, "width", object.size[0]); })
      .and_then([&] { return read_attr_to(object_json, "height", object.size[1]); })
      .and_then([&] { return read_attr_to(object_json, "visible", object.visible); })
      .and_then([&] { return _read_metadata(object_json, object.meta); })
      .transform([&] { return std::move(object); });
}

[[nodiscard]]
auto _read_object_layer(const JSON& layer_json, std::vector<ir::Object>& objects)
    -> std::expected<void, ErrorCode>
{
  return read_array<ir::Object>(layer_json, "objects", &_read_object)
      .and_then([&](std::vector<ir::Object>&& read_objects) {
        objects = std::move(read_objects);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_group_layer(const IRuntime& runtime,
                       const JSON& layer_json,
                       ir::Layer& layer,
                       ir::TileFormat& tile_format) -> std::expected<void, ErrorCode>
{
  return _read_layers(runtime, layer_json, layer.layers, tile_format);
}

[[nodiscard]]
auto _read_layer(const IRuntime& runtime, const JSON& layer_json, ir::TileFormat& tile_format)
    -> std::expected<ir::Layer, ErrorCode>
{
  ir::Layer layer {};
  return read_attr_to(layer_json, "id", layer.id)
      .and_then([&] { return read_attr_to(layer_json, "name", layer.meta.name); })
      .and_then([&] {
        layer.opacity = read_attr<float>(layer_json, "opacity").value_or(1.0f);
        return std::expected<void, ErrorCode> {};
      })
      .and_then([&] {
        layer.visible = read_attr<bool>(layer_json, "visible").value_or(true);
        return std::expected<void, ErrorCode> {};
      })
      .and_then([&] { return read_attr<std::string>(layer_json, "type"); })
      .and_then([&](const std::string& type) { return read_layer_type(type); })
      .and_then([&](const LayerType type) {
        layer.type = type;
        switch (type) {
          case LayerType::kTileLayer: {
            return _read_tile_layer(runtime, layer_json, layer, tile_format);
          }
          case LayerType::kObjectLayer: {
            return _read_object_layer(layer_json, layer.objects);
          }
          case LayerType::kGroupLayer: {
            return _read_group_layer(runtime, layer_json, layer, tile_format);
          }
          default: throw std::invalid_argument {"bad layer type"};
        }
      })
      .and_then([&] { return _read_metadata(layer_json, layer.meta); })
      .transform([&] { return std::move(layer); });
}

[[nodiscard]]
auto _read_layers(const IRuntime& runtime,
                  const JSON& root_json,
                  std::vector<ir::Layer>& layers,
                  ir::TileFormat& tile_format) -> std::expected<void, ErrorCode>
{
  const auto layer_parser = [&](const JSON& layer_json) {
    return _read_layer(runtime, layer_json, tile_format);
  };

  return read_array<ir::Layer>(root_json, "layers", layer_parser)
      .and_then([&](std::vector<ir::Layer>&& read_layers) {
        layers = std::move(read_layers);
        return std::expected<void, ErrorCode> {};
      });
}

}  // namespace

auto parse_tmj_map(const IRuntime& runtime,
                   const JSON& map_json,
                   const SaveFormatReadOptions& options) -> std::expected<ir::Map, ErrorCode>
{
  ir::Map map {};

  if (read_attr<std::string>(map_json, "orientation") != "orthogonal") {
    runtime::log(LogLevel::kError, "Unsupported map orientation");
    return std::unexpected {ErrorCode::kNotSupported};
  }

  return read_attr_to(map_json, "width", map.extent.cols)
      .and_then([&] { return read_attr_to(map_json, "height", map.extent.rows); })
      .and_then([&] { return read_attr_to(map_json, "tilewidth", map.tile_size[0]); })
      .and_then([&] { return read_attr_to(map_json, "tileheight", map.tile_size[1]); })
      .and_then([&] { return read_attr_to(map_json, "nextlayerid", map.next_layer_id); })
      .and_then([&] { return read_attr_to(map_json, "nextobjectid", map.next_object_id); })
      .and_then([&] { return _read_metadata(map_json, map.meta); })
      .and_then([&] { return _read_tilesets(map_json, options, map); })
      .and_then([&] { return _read_layers(runtime, map_json, map.layers, map.tile_format); })
      .transform([&] { return std::move(map); });
}

}  // namespace tactile::tiled_tmj
