// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <concepts>     // integral, invocable
#include <expected>     // expected, unexpected
#include <filesystem>   // path
#include <optional>     // optional
#include <string>       // string
#include <string_view>  // string_view
#include <utility>      // move
#include <vector>       // vector

#include <nlohmann/json.hpp>

#include "tactile/base/debug/error_code.hpp"
#include "tactile/base/io/compress/compression_format_id.hpp"
#include "tactile/base/layer/layer_type.hpp"
#include "tactile/base/layer/tile_encoding.hpp"
#include "tactile/base/meta/attribute_type.hpp"
#include "tactile/base/numeric/conversion.hpp"
#include "tactile/tiled_tmj/api.hpp"
#include "tactile/tiled_tmj/logging.hpp"

namespace tactile::tiled_tmj {

using JSON = nlohmann::json;

template <typename T>
struct JsonRepr;

template <>
struct JsonRepr<const char*> final
{
  using type = JSON::string_t;
};

template <>
struct JsonRepr<std::string> final
{
  using type = JSON::string_t;
};

template <>
struct JsonRepr<signed char> final
{
  using type = JSON::number_integer_t;
};

template <>
struct JsonRepr<signed short> final
{
  using type = JSON::number_integer_t;
};

template <>
struct JsonRepr<signed int> final
{
  using type = JSON::number_integer_t;
};

template <>
struct JsonRepr<signed long> final
{
  using type = JSON::number_integer_t;
};

template <>
struct JsonRepr<signed long long> final
{
  using type = JSON::number_integer_t;
};

template <>
struct JsonRepr<unsigned char> final
{
  using type = JSON::number_unsigned_t;
};

template <>
struct JsonRepr<unsigned short> final
{
  using type = JSON::number_unsigned_t;
};

template <>
struct JsonRepr<unsigned int> final
{
  using type = JSON::number_unsigned_t;
};

template <>
struct JsonRepr<unsigned long> final
{
  using type = JSON::number_unsigned_t;
};

template <>
struct JsonRepr<unsigned long long> final
{
  using type = JSON::number_unsigned_t;
};

template <>
struct JsonRepr<float> final
{
  using type = JSON::number_float_t;
};

template <>
struct JsonRepr<double> final
{
  using type = JSON::number_float_t;
};

template <>
struct JsonRepr<bool> final
{
  using type = JSON::boolean_t;
};

template <typename T>
using JsonReprOf = typename JsonRepr<T>::type;

template <typename T>
[[nodiscard]] auto read_attr(const JSON& json_object, const char* name)
    -> std::expected<T, ErrorCode>
{
  const auto attr_iter = json_object.find(name);
  if (attr_iter == json_object.end()) {
    // This is pretty common, so we don't log it as a hard error.
    TACTILE_TILED_TMJ_TRACE("Could not find JSON attribute '{}'", name);
    return std::unexpected {ErrorCode::kParseError};
  }

  const auto* attr_value = attr_iter->get_ptr<const JsonReprOf<T>*>();
  if (!attr_value) {
    TACTILE_TILED_TMJ_ERROR("Unexpected type of JSON attribute '{}'", name);
    return std::unexpected {ErrorCode::kParseError};
  }

  if constexpr (std::integral<T>) {
    return narrow<T>(*attr_value);
  }
  else {
    return static_cast<T>(*attr_value);
  }
}

template <typename T>
[[nodiscard]] auto read_attr_to(const JSON& json_object, const char* name, T& value)
    -> std::expected<void, ErrorCode>
{
  return read_attr<T>(json_object, name)
      .and_then([&](T&& result) -> std::expected<void, ErrorCode> {
        value = std::move(result);
        return {};
      });
}

template <typename T, std::invocable<const JSON&> Parser>
[[nodiscard]] auto read_array(const JSON& json,
                              const char* array_name,
                              const Parser& value_parser)
    -> std::expected<std::vector<T>, ErrorCode>
{
  std::vector<T> values {};

  const auto array_iter = json.find(array_name);
  if (array_iter == json.end()) {
    // Most arrays in the TMJ format aren't listed as optional, but are often omitted by Tiled.
    return values;
  }

  values.reserve(array_iter->size());

  for (const auto& [_, value_json] : array_iter->items()) {
    auto value = value_parser(value_json);

    if (!value.has_value()) {
      return std::unexpected {value.error()};
    }

    values.push_back(std::move(*value));
  }

  return values;
}

[[nodiscard]]
TACTILE_TILED_TMJ_API auto read_json_document(const std::filesystem::path& path)
    -> std::expected<JSON, ErrorCode>;

[[nodiscard]]
TACTILE_TILED_TMJ_API auto read_property_type(std::string_view name)
    -> std::expected<AttributeType, ErrorCode>;

[[nodiscard]]
TACTILE_TILED_TMJ_API auto read_layer_type(std::string_view name)
    -> std::expected<LayerType, ErrorCode>;

[[nodiscard]]
TACTILE_TILED_TMJ_API auto read_tile_encoding(std::string_view name)
    -> std::expected<TileEncoding, ErrorCode>;

[[nodiscard]]
TACTILE_TILED_TMJ_API auto read_compression_format(std::string_view name)
    -> std::expected<std::optional<CompressionFormatId>, ErrorCode>;

}  // namespace tactile::tiled_tmj
