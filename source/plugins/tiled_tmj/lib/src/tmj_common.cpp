// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmj/tmj_common.hpp"

#include <exception>  // exception
#include <fstream>    // ifstream
#include <ios>        // ios

namespace tactile::tiled_tmj {

auto read_json_document(const std::filesystem::path& path) -> std::expected<JSON, ErrorCode>
{
  try {
    std::ifstream stream {path, std::ios::in};
    if (!stream.good()) {
      runtime::log(LogLevel::kError, "Could not open JSON document: {}", path.string());
      return std::unexpected {ErrorCode::kBadFileStream};
    }

    JSON json {};
    stream >> json;

    return json;
  }
  catch (const std::exception& error) {
    runtime::log(LogLevel::kError, "JSON parse error: {}", error.what());
    return std::unexpected {ErrorCode::kParseError};
  }
  catch (...) {
    runtime::log(LogLevel::kError, "Unknown JSON parse error");
    return std::unexpected {ErrorCode::kParseError};
  }
}

auto read_property_type(std::string_view name) -> std::expected<AttributeType, ErrorCode>
{
  if (name == "string") {
    return AttributeType::kStr;
  }

  if (name == "int") {
    return AttributeType::kInt;
  }

  if (name == "float") {
    return AttributeType::kFloat;
  }

  if (name == "bool") {
    return AttributeType::kBool;
  }

  if (name == "color") {
    return AttributeType::kColor;
  }

  if (name == "file") {
    return AttributeType::kPath;
  }

  if (name == "object") {
    return AttributeType::kObject;
  }

  runtime::log(LogLevel::kError, "Unsupported property type '{}'", name);
  return std::unexpected {ErrorCode::kNotSupported};
}

auto read_layer_type(const std::string_view name) -> std::expected<LayerType, ErrorCode>
{
  if (name == "tilelayer") {
    return LayerType::kTileLayer;
  }

  if (name == "objectgroup") {
    return LayerType::kObjectLayer;
  }

  if (name == "group") {
    return LayerType::kGroupLayer;
  }

  runtime::log(LogLevel::kError, "Unsupported layer type '{}'", name);
  return std::unexpected {ErrorCode::kNotSupported};
}

auto read_tile_encoding(const std::string_view name) -> std::expected<TileEncoding, ErrorCode>
{
  if (name == "csv") {
    return TileEncoding::kPlainText;
  }

  if (name == "base64") {
    return TileEncoding::kBase64;
  }

  runtime::log(LogLevel::kError, "Unsupported tile encoding '{}'", name);
  return std::unexpected {ErrorCode::kNotSupported};
}

auto read_compression_format(const std::string_view name)
    -> std::expected<std::optional<CompressionFormatId>, ErrorCode>
{
  if (name.empty()) {
    return std::nullopt;  // FIXME check if valid
  }

  if (name == "zlib") {
    return CompressionFormatId::kZlib;
  }

  if (name == "zstd") {
    return CompressionFormatId::kZstd;
  }

  runtime::log(LogLevel::kError, "Unsupported compression format '{}'", name);
  return std::unexpected {ErrorCode::kNotSupported};
}

}  // namespace tactile::tiled_tmj
