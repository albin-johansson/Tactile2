#pragma once

#include <QFileInfo>      // QFileInfo
#include <QJsonArray>     // QJsonArray
#include <QJsonDocument>  // QJsonDocument
#include <QJsonValue>     // QJsonValue
#include <vector>         // vector

#include "json_element.hpp"
#include "map_parse_data.hpp"
#include "parse_error.hpp"
#include "tile_layer.hpp"

namespace tactile::parse {

class json_engine final
{
 public:
  using document_type = QJsonDocument;
  using object_type = json_element;

  // clang-format off

  [[nodiscard]]
  static auto root(const document_type& document) -> object_type;

  [[nodiscard]]
  static auto from_file(const QFileInfo& path) -> maybe<document_type>;

  [[nodiscard]]
  static auto tilesets(const object_type& root) -> std::vector<object_type>;

  [[nodiscard]]
  static auto layers(const object_type& root) -> std::vector<object_type>;

  [[nodiscard]]
  static auto properties(const object_type& object) -> std::vector<object_type>;

  [[nodiscard]]
  static auto add_tiles(tile_layer_data& layer,
                        const object_type& element,
                        parse_error& error) -> bool;

  [[nodiscard]]
  static auto contains_tilesets(const object_type& object) -> bool;

  [[nodiscard]]
  static auto tileset_image_relative_path(const object_type& object)
      -> maybe<QString>;

  [[nodiscard]]
  static auto validate_layer_type(const object_type& object) -> bool;

  [[nodiscard]]
  static auto contains_layers(const object_type& object) -> bool;

  [[nodiscard]]
  static auto assume_string_property(const object_type& object) -> bool;

  [[nodiscard]]
  static auto is_tile_layer(const object_type& object) -> bool;

  [[nodiscard]]
  static auto is_object_layer(const object_type& object) -> bool;

  // clang-format on

 private:
  static auto collect(const object_type& root, QStringView key)
      -> std::vector<object_type>;
};

}  // namespace tactile::parse
