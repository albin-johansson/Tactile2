// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmx/tmx_format_parser.hpp"

#include <array>        // array
#include <charconv>     // from_chars
#include <cstddef>      // size_t
#include <cstdint>      // uint8_t
#include <cstring>      // strcmp
#include <optional>     // optional
#include <stdexcept>    // invalid_argument
#include <string>       // string
#include <string_view>  // string_view
#include <utility>      // move

#include <cppcodec/base64_default_rfc4648.hpp>
#include <pugixml.hpp>

#include "tactile/base/container/string.hpp"
#include "tactile/base/io/compress/compression_format.hpp"
#include "tactile/base/io/tile_io.hpp"
#include "tactile/base/util/tile_matrix.hpp"
#include "tactile/tiled_tmx/logging.hpp"
#include "tactile/tiled_tmx/tmx_common.hpp"

namespace tactile::tiled_tmx {
namespace {

enum class TmxTileEncoding : std::uint8_t
{
  kTileNodes,
  kCsv,
  kBase64,
};

[[nodiscard]]
auto _read_property(const pugi::xml_node& node, const AttributeType type)
    -> std::expected<Attribute, ErrorCode>
{
  switch (type) {
    case AttributeType::kStr: {
      if (auto read_value = read_attr<std::string>(node, "value")) {
        return Attribute {std::move(*read_value)};
      }
      break;
    }
    case AttributeType::kInt: {
      if (const auto read_value = read_attr<Attribute::int_type>(node, "value")) {
        return Attribute {*read_value};
      }
      break;
    }
    case AttributeType::kFloat: {
      if (const auto read_value = read_attr<Attribute::float_type>(node, "value")) {
        return Attribute {*read_value};
      }
      break;
    }
    case AttributeType::kBool: {
      if (const auto read_value = read_attr<bool>(node, "value")) {
        return Attribute {*read_value};
      }
      break;
    }
    case AttributeType::kPath: {
      if (const auto read_value = read_attr<std::string>(node, "value")) {
        return Attribute {Attribute::path_type {*read_value}};
      }
      break;
    }
    case AttributeType::kColor: {
      if (const auto read_value = read_attr<std::string>(node, "value")) {
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
              read_attr<Attribute::objref_type::value_type>(node, "value")) {
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
auto _read_named_attribute(const pugi::xml_node& property_node)
    -> std::expected<ir::NamedAttribute, ErrorCode>
{
  ir::NamedAttribute named_attribute {};

  return read_attr_to(property_node, "name", named_attribute.name)
      .and_then([&] {
        // TMX properties are implicitly strings if type is missing
        const char* type_name = property_node.attribute("type").as_string("string");
        return read_property_type(type_name);
      })
      .and_then([&](const AttributeType property_type) {
        return _read_property(property_node, property_type);
      })
      .transform([&](Attribute&& value) {
        named_attribute.value = std::move(value);
        return std::move(named_attribute);
      });
}

[[nodiscard]]
auto _read_metadata(const pugi::xml_node& node, ir::Metadata& meta)
    -> std::expected<void, ErrorCode>
{
  const auto properties_node = node.child("properties");
  return read_nodes<ir::NamedAttribute>(properties_node, "property", &_read_named_attribute)
      .and_then([&](std::vector<ir::NamedAttribute>&& properties) {
        meta.properties = std::move(properties);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_tileset_image_node(const pugi::xml_node& image_node, ir::Tileset& tileset)
    -> std::expected<void, ErrorCode>
{
  std::string source {};
  return read_attr_to(image_node, "width", tileset.image_size[0])
      .and_then([&] { return read_attr_to(image_node, "height", tileset.image_size[1]); })
      .and_then([&] { return read_attr_to(image_node, "source", source); })
      .and_then([&] {
        tileset.image_path = source;
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_animation_frame(const pugi::xml_node& frame_node)
    -> std::expected<ir::AnimationFrame, ErrorCode>
{
  ir::AnimationFrame frame {};
  std::chrono::milliseconds::rep duration {};

  return read_attr_to(frame_node, "tileid", frame.tile_index)
      .and_then([&] { return read_attr_to(frame_node, "duration", duration); })
      .transform([&] {
        frame.duration = std::chrono::milliseconds {duration};
        return std::move(frame);
      });
}

[[nodiscard]]
auto _read_tile_animation(const pugi::xml_node& animation_node, ir::Tile& tile)
    -> std::expected<void, ErrorCode>
{
  return read_nodes<ir::AnimationFrame>(animation_node, "frame", &_read_animation_frame)
      .and_then([&](std::vector<ir::AnimationFrame>&& frames) {
        tile.animation = std::move(frames);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_object_layer(const pugi::xml_node& layer_node, std::vector<ir::Object>& objects)
    -> std::expected<void, ErrorCode>;

[[nodiscard]]
auto _read_tileset_tile(const pugi::xml_node& tile_node) -> std::expected<ir::Tile, ErrorCode>
{
  ir::Tile tile {};
  return read_attr_to(tile_node, "id", tile.index)
      .and_then([&] {
        const auto animation_node = tile_node.child("animation");
        return _read_tile_animation(animation_node, tile);
      })
      .and_then([&] { return _read_metadata(tile_node, tile.meta); })
      .and_then([&] {
        const auto layer_node = tile_node.child("objectgroup");
        return _read_object_layer(layer_node, tile.objects);
      })
      .transform([&] { return std::move(tile); });
}

[[nodiscard]]
auto _read_tileset_tiles(const pugi::xml_node& tileset_node, ir::Tileset& tileset)
    -> std::expected<void, ErrorCode>
{
  return read_nodes<ir::Tile>(tileset_node, "tile", &_read_tileset_tile)
      .and_then([&](std::vector<ir::Tile>&& tiles) {
        tileset.tiles = std::move(tiles);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_common_tileset_attributes(const pugi::xml_node& tileset_node, ir::Tileset& tileset)
    -> std::expected<void, ErrorCode>
{
  return _read_metadata(tileset_node, tileset.meta)
      .and_then([&] { return read_attr_to(tileset_node, "name", tileset.meta.name); })
      .and_then([&] { return read_attr_to(tileset_node, "tilewidth", tileset.tile_size[0]); })
      .and_then([&] { return read_attr_to(tileset_node, "tileheight", tileset.tile_size[1]); })
      .and_then([&] { return read_attr_to(tileset_node, "tilecount", tileset.tile_count); })
      .and_then([&] { return read_attr_to(tileset_node, "columns", tileset.column_count); })
      .and_then([&] {
        const auto image_node = tileset_node.child("image");
        return _read_tileset_image_node(image_node, tileset);
      })
      .and_then([&] { return _read_tileset_tiles(tileset_node, tileset); });
}

[[nodiscard]]
auto _read_embedded_tileset(const pugi::xml_node& tileset_node)
    -> std::expected<ir::Tileset, ErrorCode>
{
  ir::Tileset tileset {};
  tileset.is_embedded = true;

  return _read_common_tileset_attributes(tileset_node, tileset).transform([&] {
    return std::move(tileset);
  });
}

[[nodiscard]]
auto _read_external_tileset(const std::filesystem::path& path)
    -> std::expected<ir::Tileset, ErrorCode>
{
  ir::Tileset tileset {};
  tileset.is_embedded = false;

  return read_xml_document(path)
      .and_then([&](const pugi::xml_document& document) -> std::expected<void, ErrorCode> {
        const auto tileset_node = document.child("tileset");
        return _read_common_tileset_attributes(tileset_node, tileset);
      })
      .transform([&] { return std::move(tileset); });
}

[[nodiscard]]
auto _read_tileset_ref(const pugi::xml_node& tileset_ref_node,
                       const SaveFormatReadOptions& options)
    -> std::expected<ir::TilesetRef, ErrorCode>
{
  ir::TilesetRef tileset_ref {};
  return read_attr_to(tileset_ref_node, "firstgid", tileset_ref.first_tile_id)
      .and_then([&] {
        const auto source = read_attr<std::string>(tileset_ref_node, "source");
        return source.has_value() ? _read_external_tileset(options.base_dir / *source)
                                  : _read_embedded_tileset(tileset_ref_node);
      })
      .transform([&](ir::Tileset&& tileset) {
        tileset_ref.tileset = std::move(tileset);
        return std::move(tileset_ref);
      });
}

[[nodiscard]]
auto _read_tilesets(const pugi::xml_node& map_node,
                    const SaveFormatReadOptions& options,
                    ir::Map& map) -> std::expected<void, ErrorCode>
{
  const auto tileset_ref_parser = [&](const pugi::xml_node& tileset_ref_node) {
    return _read_tileset_ref(tileset_ref_node, options);
  };

  return read_nodes<ir::TilesetRef>(map_node, "tileset", tileset_ref_parser)
      .and_then([&](std::vector<ir::TilesetRef>&& tileset_refs) {
        map.tilesets = std::move(tileset_refs);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_tile_layer_data_encoding(const pugi::xml_node& data_node)
    -> std::expected<TmxTileEncoding, ErrorCode>
{
  const auto* encoding = data_node.attribute("encoding").as_string(nullptr);

  if (encoding != nullptr) {
    if (std::strcmp(encoding, "csv") == 0) {
      return TmxTileEncoding::kCsv;
    }

    if (std::strcmp(encoding, "base64") == 0) {
      return TmxTileEncoding::kBase64;
    }

    TACTILE_TILED_TMX_ERROR("Unsupported tile encoding '{}'", encoding);
    return std::unexpected {ErrorCode::kNotSupported};
  }

  return TmxTileEncoding::kTileNodes;
}

[[nodiscard]]
auto _read_tile_nodes_data(const pugi::xml_node& data_node, const Extent2D& extent)
    -> std::expected<TileMatrix, ErrorCode>
{
  auto tile_matrix = make_tile_matrix(extent);

  std::size_t index {0};
  for (const auto& tile_node : data_node.children("tile")) {
    const auto position = Index2D::from_1d(index, extent.cols);

    const auto read_result =
        read_attr_to(tile_node, "gid", tile_matrix[position.y][position.x]);
    if (!read_result.has_value()) {
      return std::unexpected {read_result.error()};
    }

    ++index;
  }

  return tile_matrix;
}

[[nodiscard]]
auto _read_csv_tile_data(const pugi::xml_node& data_node, const Extent2D& extent)
    -> std::expected<TileMatrix, ErrorCode>
{
  const auto data_node_text = data_node.text();

  auto tile_matrix = make_tile_matrix(extent);
  std::size_t tile_index {};

  const auto split_ok =
      visit_tokens(data_node_text.get(), '\n', [&](const std::string_view csv_row) {
        return visit_tokens(csv_row, ',', [&](const std::string_view token) {
          TileID tile_id {};

          const auto parse_tile_id_result =
              std::from_chars(token.data(), token.data() + token.size(), tile_id);
          if (parse_tile_id_result.ec != std::errc {}) {
            return false;
          }

          const auto position = Index2D::from_1d(tile_index, extent.cols);
          tile_matrix[position.y][position.x] = tile_id;

          ++tile_index;
          return true;
        });
      });

  if (!split_ok) {
    TACTILE_TILED_TMX_ERROR("Could not parse CSV tile data");
    return std::unexpected {ErrorCode::kParseError};
  }

  return tile_matrix;
}

[[nodiscard]]
auto _read_base64_tile_data(const IRuntime& runtime,
                            const pugi::xml_node& data_node,
                            const Extent2D& extent,
                            ir::TileFormat& tile_format)
    -> std::expected<TileMatrix, ErrorCode>
{
  const char* compression = data_node.attribute("compression").as_string();

  const auto compression_format_id = read_compression_format(compression);
  if (!compression_format_id.has_value()) {
    return std::unexpected {compression_format_id.error()};
  }

  tile_format.encoding = TileEncoding::kBase64;
  tile_format.compression = *compression_format_id;

  const auto data_node_text = data_node.text();
  const std::string_view encoded_tile_data {data_node_text.get()};

  auto decoded_tile_data = base64::decode(encoded_tile_data);

  ByteStream raw_tile_matrix {};
  if (tile_format.compression.has_value()) {
    const auto* compression_format = runtime.get_compression_format(*tile_format.compression);

    if (!compression_format) {
      TACTILE_TILED_TMX_ERROR("No suitable compression plugin available");
      return std::unexpected {ErrorCode::kNotSupported};
    }

    const auto decompressed_tile_data = compression_format->decompress(decoded_tile_data);
    if (!decompressed_tile_data.has_value()) {
      return std::unexpected {decompressed_tile_data.error()};
    }

    raw_tile_matrix = std::move(*decompressed_tile_data);
  }
  else {
    raw_tile_matrix = std::move(decoded_tile_data);
  }

  auto tile_matrix = parse_raw_tile_matrix(raw_tile_matrix, extent, TileIdFormat::kTiled);
  if (!tile_matrix) {
    return std::unexpected {ErrorCode::kParseError};
  }

  return std::move(*tile_matrix);
}

[[nodiscard]]
auto _read_tile_layer_data(const IRuntime& runtime,
                           const pugi::xml_node& data_node,
                           ir::Layer& layer,
                           ir::TileFormat& tile_format) -> std::expected<void, ErrorCode>
{
  tile_format.encoding = TileEncoding::kPlainText;
  tile_format.compression = std::nullopt;

  return _read_tile_layer_data_encoding(data_node)
      .and_then([&](const TmxTileEncoding encoding) {
        switch (encoding) {
          case TmxTileEncoding::kTileNodes: {
            return _read_tile_nodes_data(data_node, layer.extent);
          }
          case TmxTileEncoding::kCsv: {
            return _read_csv_tile_data(data_node, layer.extent);
          }
          case TmxTileEncoding::kBase64: {
            return _read_base64_tile_data(runtime, data_node, layer.extent, tile_format);
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
auto _read_tile_layer(const IRuntime& runtime,
                      const pugi::xml_node& layer_node,
                      ir::Layer& layer,
                      ir::TileFormat& tile_format) -> std::expected<void, ErrorCode>
{
  return read_attr_to(layer_node, "width", layer.extent.cols)
      .and_then([&] { return read_attr_to(layer_node, "height", layer.extent.rows); })
      .and_then([&] {
        const auto data_node = layer_node.child("data");
        return _read_tile_layer_data(runtime, data_node, layer, tile_format);
      });
}

[[nodiscard]]
auto _read_object(const pugi::xml_node& object_node) -> std::expected<ir::Object, ErrorCode>
{
  ir::Object object {};

  object.meta.name = read_attr<std::string>(object_node, "name").value_or("");
  object.tag = read_attr<std::string>(object_node, "type").value_or("");
  object.position[0] = read_attr<float>(object_node, "x").value_or(0.0f);
  object.position[1] = read_attr<float>(object_node, "y").value_or(0.0f);
  object.size[0] = read_attr<float>(object_node, "width").value_or(0.0f);
  object.size[1] = read_attr<float>(object_node, "height").value_or(0.0f);
  object.visible = read_attr<bool>(object_node, "visible").value_or(true);

  if (!object_node.child("point").empty()) {
    object.type = ObjectType::kPoint;
  }
  else if (!object_node.child("ellipse").empty()) {
    object.type = ObjectType::kEllipse;
  }
  else {
    object.type = ObjectType::kRect;
  }

  return read_attr_to(object_node, "id", object.id)
      .and_then([&] { return _read_metadata(object_node, object.meta); })
      .transform([&] { return std::move(object); });
}

[[nodiscard]]
auto _read_object_layer(const pugi::xml_node& layer_node, std::vector<ir::Object>& objects)
    -> std::expected<void, ErrorCode>
{
  return read_nodes<ir::Object>(layer_node, "object", &_read_object)
      .and_then([&](std::vector<ir::Object>&& read_objects) {
        objects = std::move(read_objects);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_layers(const IRuntime& runtime,
                  const pugi::xml_node& root_node,
                  std::vector<ir::Layer>& layers,
                  ir::TileFormat& tile_format) -> std::expected<void, ErrorCode>;

[[nodiscard]]
auto _read_group_layer(const IRuntime& runtime,
                       const pugi::xml_node& layer_node,
                       ir::Layer& layer,
                       ir::TileFormat& tile_format) -> std::expected<void, ErrorCode>
{
  return _read_layers(runtime, layer_node, layer.layers, tile_format);
}

[[nodiscard]]
auto _read_layer(const IRuntime& runtime,
                 const pugi::xml_node& layer_node,
                 ir::TileFormat& tile_format) -> std::expected<ir::Layer, ErrorCode>
{
  ir::Layer layer {};
  return read_attr_to(layer_node, "id", layer.id)
      .and_then([&] { return read_attr_to(layer_node, "name", layer.meta.name); })
      .and_then([&] { return read_attr_to(layer_node, "name", layer.meta.name); })
      .and_then([&] {
        layer.opacity = read_attr<float>(layer_node, "opacity").value_or(1.0f);
        return std::expected<void, ErrorCode> {};
      })
      .and_then([&] {
        layer.visible = read_attr<bool>(layer_node, "visible").value_or(true);
        return std::expected<void, ErrorCode> {};
      })
      .and_then([&] { return read_layer_type(layer_node.name()); })
      .and_then([&](const LayerType type) {
        layer.type = type;
        switch (type) {
          case LayerType::kTileLayer: {
            return _read_tile_layer(runtime, layer_node, layer, tile_format);
          }
          case LayerType::kObjectLayer: {
            return _read_object_layer(layer_node, layer.objects);
          }
          case LayerType::kGroupLayer: {
            return _read_group_layer(runtime, layer_node, layer, tile_format);
          }
          default: throw std::invalid_argument {"bad layer type"};
        }
      })
      .and_then([&] { return _read_metadata(layer_node, layer.meta); })
      .transform([&] { return std::move(layer); });
}

[[nodiscard]]
auto _read_layers(const IRuntime& runtime,
                  const pugi::xml_node& root_node,
                  std::vector<ir::Layer>& layers,
                  ir::TileFormat& tile_format) -> std::expected<void, ErrorCode>
{
  using namespace std::string_view_literals;
  constexpr std::array layer_node_names = {"layer"sv, "objectgroup"sv, "group"sv};

  const auto layer_parser =
      [&](const pugi::xml_node& layer_node) -> std::expected<ir::Layer, ErrorCode> {
    return _read_layer(runtime, layer_node, tile_format);
  };

  return read_nodes<ir::Layer>(root_node, layer_node_names, layer_parser)
      .and_then([&](std::vector<ir::Layer>&& read_layers) {
        layers = std::move(read_layers);
        return std::expected<void, ErrorCode> {};
      });
}

[[nodiscard]]
auto _read_map(std::string map_name,
               const IRuntime& runtime,
               const pugi::xml_node& map_node,
               const SaveFormatReadOptions& options) -> std::expected<ir::Map, ErrorCode>
{
  ir::Map map {};
  map.meta.name = std::move(map_name);

  if (read_attr<std::string>(map_node, "orientation") != "orthogonal") {
    TACTILE_TILED_TMX_ERROR("Non-orthogonal maps are not supported");
    return std::unexpected {ErrorCode::kNotSupported};
  }

  if (read_attr<bool>(map_node, "infinite").value_or(false)) {
    TACTILE_TILED_TMX_ERROR("Infinite maps are not supported");
    return std::unexpected {ErrorCode::kNotSupported};
  }

  return read_attr_to(map_node, "tilewidth", map.tile_size[0])
      .and_then([&] { return read_attr_to(map_node, "tileheight", map.tile_size[1]); })
      .and_then([&] { return read_attr_to(map_node, "width", map.extent.cols); })
      .and_then([&] { return read_attr_to(map_node, "height", map.extent.rows); })
      .and_then([&] { return read_attr_to(map_node, "nextlayerid", map.next_layer_id); })
      .and_then([&] { return read_attr_to(map_node, "nextobjectid", map.next_object_id); })
      .and_then([&] { return _read_tilesets(map_node, options, map); })
      .and_then([&] { return _read_layers(runtime, map_node, map.layers, map.tile_format); })
      .and_then([&] { return _read_metadata(map_node, map.meta); })
      .transform([&] { return std::move(map); });
}

}  // namespace

auto parse_map(const IRuntime& runtime,
               const std::filesystem::path& map_path,
               const SaveFormatReadOptions& options) -> std::expected<ir::Map, ErrorCode>
{
  return read_xml_document(map_path).and_then([&](const pugi::xml_document& map_document) {
    const auto map_node = map_document.child("map");
    return _read_map(map_path.filename().string(), runtime, map_node, options);
  });
}

}  // namespace tactile::tiled_tmx
