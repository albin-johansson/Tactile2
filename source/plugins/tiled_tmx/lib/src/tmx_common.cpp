// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile//tiled_tmx/tmx_common.hpp"

#include <fstream>    // ifstream
#include <stdexcept>  // invalid_argument

#include "tactile/tiled_tmx/logging.hpp"

namespace tactile::tiled_tmx {
namespace {

inline constexpr const char* kStringPropertyTypeName = "string";
inline constexpr const char* kIntPropertyTypeName = "int";
inline constexpr const char* kFloatPropertyTypeName = "float";
inline constexpr const char* kBoolPropertyTypeName = "bool";
inline constexpr const char* kColorPropertyTypeName = "color";
inline constexpr const char* kPathPropertyTypeName = "file";
inline constexpr const char* kObjectPropertyTypeName = "object";

inline constexpr const char* kTileLayerTypeName = "layer";
inline constexpr const char* kObjectLayerTypeName = "objectgroup";
inline constexpr const char* kGroupLayerTypeName = "group";

}  // namespace

auto read_xml_document(const std::filesystem::path& path)
    -> std::expected<pugi::xml_document, ErrorCode>
{
  TACTILE_TILED_TMX_TRACE("Parsing XML document at {}", path.string());

  std::ifstream stream {path, std::ios::in};
  if (!stream.good()) {
    return std::unexpected {ErrorCode::kBadFileStream};
  }

  constexpr auto parse_options = pugi::parse_default | pugi::parse_trim_pcdata;

  pugi::xml_document xml_document {};
  const auto load_result = xml_document.load(stream, parse_options);

  if (load_result.status != pugi::status_ok) {
    TACTILE_TILED_TMX_ERROR("XML parse error: {}", load_result.description());
    return std::unexpected {ErrorCode::kParseError};
  }

  return xml_document;
}

auto save_xml_document(const pugi::xml_document& document, const std::filesystem::path& path)
    -> std::expected<void, ErrorCode>
{
  TACTILE_TILED_TMX_TRACE("Saving XML document to {}", path.string());

  if (!document.save_file(path.c_str(), "  ")) {
    TACTILE_TILED_TMX_ERROR("Could not save XML document");
    return std::unexpected {ErrorCode::kWriteError};
  }

  return {};
}

auto read_property_type(const std::string_view name) -> std::expected<AttributeType, ErrorCode>
{
  if (name == kStringPropertyTypeName) {
    return AttributeType::kStr;
  }

  if (name == kIntPropertyTypeName) {
    return AttributeType::kInt;
  }

  if (name == kFloatPropertyTypeName) {
    return AttributeType::kFloat;
  }

  if (name == kBoolPropertyTypeName) {
    return AttributeType::kBool;
  }

  if (name == kColorPropertyTypeName) {
    return AttributeType::kColor;
  }

  if (name == kPathPropertyTypeName) {
    return AttributeType::kPath;
  }

  if (name == kObjectPropertyTypeName) {
    return AttributeType::kObject;
  }

  TACTILE_TILED_TMX_ERROR("Unsupported property type '{}'", name);
  return std::unexpected {ErrorCode::kNotSupported};
}

auto get_property_type_name(const AttributeType type) -> const char*
{
  switch (type) {
    case AttributeType::kStr:
    case AttributeType::kInt2:
    case AttributeType::kInt3:
    case AttributeType::kInt4:
    case AttributeType::kFloat2:
    case AttributeType::kFloat3: [[fallthrough]];
    case AttributeType::kFloat4: return kStringPropertyTypeName;
    case AttributeType::kInt:    return kIntPropertyTypeName;
    case AttributeType::kFloat:  return kFloatPropertyTypeName;
    case AttributeType::kBool:   return kBoolPropertyTypeName;
    case AttributeType::kPath:   return kPathPropertyTypeName;
    case AttributeType::kColor:  return kColorPropertyTypeName;
    case AttributeType::kObject: return kObjectPropertyTypeName;
  }

  throw std::invalid_argument {"bad attribute type"};
}

auto read_layer_type(const std::string_view name) -> std::expected<LayerType, ErrorCode>
{
  if (name == kTileLayerTypeName) {
    return LayerType::kTileLayer;
  }

  if (name == kObjectLayerTypeName) {
    return LayerType::kObjectLayer;
  }

  if (name == kGroupLayerTypeName) {
    return LayerType::kGroupLayer;
  }

  TACTILE_TILED_TMX_ERROR("Unsupported layer type '{}'", name);
  return std::unexpected {ErrorCode::kNotSupported};
}

auto get_layer_type_name(const LayerType type) -> const char*
{
  switch (type) {
    case LayerType::kTileLayer:   return kTileLayerTypeName;
    case LayerType::kObjectLayer: return kObjectLayerTypeName;
    case LayerType::kGroupLayer:  return kGroupLayerTypeName;
  }

  throw std::invalid_argument {"bad layer type"};
}

auto read_compression_format(const std::string_view name)
    -> std::expected<std::optional<CompressionFormatId>, ErrorCode>
{
  if (name == "") {
    return std::nullopt;
  }

  if (name == "zlib") {
    return CompressionFormatId::kZlib;
  }

  if (name == "zstd") {
    return CompressionFormatId::kZstd;
  }

  TACTILE_TILED_TMX_ERROR("Unsupported compression format '{}'", name);
  return std::unexpected {ErrorCode::kNotSupported};
}

auto get_compression_format_name(const CompressionFormatId format) -> const char*
{
  switch (format) {
    case CompressionFormatId::kZlib: return "zlib";
    case CompressionFormatId::kZstd: return "zstd";
  }

  throw std::invalid_argument {"bad compression format id"};
}

}  // namespace tactile::tiled_tmx
