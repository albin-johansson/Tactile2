#pragma once

/* Convenience header that includes all other command headers */

#include "components/create_component_attribute_cmd.hpp"
#include "components/create_component_def_cmd.hpp"
#include "components/rename_component_attribute_cmd.hpp"
#include "components/rename_component_cmd.hpp"
#include "components/set_component_attribute_type_cmd.hpp"
#include "layers/add_layer_cmd.hpp"
#include "layers/duplicate_layer_cmd.hpp"
#include "layers/move_layer_down_cmd.hpp"
#include "layers/move_layer_up_cmd.hpp"
#include "layers/remove_layer_cmd.hpp"
#include "layers/rename_layer_cmd.hpp"
#include "layers/set_layer_opacity_cmd.hpp"
#include "layers/set_layer_visibility_cmd.hpp"
#include "maps/add_column_cmd.hpp"
#include "maps/add_row_cmd.hpp"
#include "maps/remove_column_cmd.hpp"
#include "maps/remove_row_cmd.hpp"
#include "maps/resize_map_cmd.hpp"
#include "objects/move_object_cmd.hpp"
#include "objects/set_object_name_cmd.hpp"
#include "objects/set_object_tag_cmd.hpp"
#include "objects/set_object_visibility_cmd.hpp"
#include "properties/add_property_cmd.hpp"
#include "properties/change_property_type_cmd.hpp"
#include "properties/remove_property_cmd.hpp"
#include "properties/rename_property_cmd.hpp"
#include "properties/update_property_cmd.hpp"
#include "tilesets/add_tileset_cmd.hpp"
#include "tilesets/remove_tileset_cmd.hpp"
#include "tilesets/set_tileset_name_cmd.hpp"
#include "tools/bucket_cmd.hpp"
#include "tools/eraser_sequence_cmd.hpp"
#include "tools/stamp_sequence_cmd.hpp"