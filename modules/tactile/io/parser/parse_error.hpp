#pragma once

#include <ostream>      // ostream
#include <string_view>  // string_view

namespace tactile::parsing {

/**
 * \brief Represents various possible errors related to map parsing.
 *
 * \todo Split into errors/"soft" errors, e.g. `parse_warning`?
 *
 * \see `to_cause(parse_error)`
 */
enum class parse_error {
  none,
  unknown,

  could_not_read_file,

  map_does_not_exist,
  unsupported_map_extension,
  unsupported_map_orientation,
  unsupported_infinite_map,
  no_map_width,
  no_map_height,
  no_map_tile_width,
  no_map_tile_height,
  no_map_next_layer_id,
  no_map_next_object_id,
  no_map_tilesets,  // Tiled JSON only (TODO warning only?)
  no_map_layers,    // Tiled JSON only (TODO warning only?)

  unsupported_layer_type,
  no_layer_id,
  no_layer_type,
  no_tile_layer_width,   // TODO warning only?
  no_tile_layer_height,  // TODO warning only?
  no_tile_layer_data,
  corrupt_tile_layer_data,
  unsupported_tile_layer_encoding,

  external_tileset_does_not_exist,
  could_not_read_external_tileset,  // TODO unknown_external_tileset_error
  no_tileset_first_tile_id,
  no_tileset_tile_width,
  no_tileset_tile_height,
  no_tileset_name,  // TODO warning only?
  no_tileset_image_path,
  no_tileset_image_width,
  no_tileset_image_height,
  no_tileset_tile_count,
  no_tileset_column_count,
  no_tileset_version,        // Tactile YAML only
  no_external_tileset_path,  // Tactile YAML only
  tileset_image_does_not_exist,
  unsupported_tileset_version,  // Tactile YAML only

  could_not_parse_property,
  no_property_name,
  no_property_type,  // Not used by Tiled XML
  unsupported_property_type,

  no_component_def_name,                      // Tactile YAML only
  no_component_def_attribute_name,            // Tactile YAML only
  no_component_def_attribute_type,            // Tactile YAML only
  unsupported_component_def_attribute_type,   // Tactile YAML only
  unsupported_component_def_attribute_value,  // Tactile YAML only

  no_component_type,                      // Tactile YAML only
  no_component_attribute_name,            // Tactile YAML only
  no_component_attribute_value,           // Tactile YAML only
  unsupported_component_attribute_value,  // Tactile YAML only (TODO corrupt_?)

  no_object_id,
  no_object_type,           // Tactile YAML only
  unsupported_object_type,  // Tactile YAML only

  no_fancy_tile_id,
  no_animation_frame_tile,
  no_animation_frame_duration
};

[[nodiscard]] auto to_cause(parse_error error) -> std::string_view;

auto operator<<(std::ostream& stream, parse_error error) -> std::ostream&;

}  // namespace tactile::parsing