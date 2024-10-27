project(tactile-plugins-godot-tscn-lib CXX)

add_library(tactile-godot-tscn SHARED)
add_library(tactile::godot_tscn ALIAS tactile-godot-tscn)

target_sources(tactile-godot-tscn
               PRIVATE
               "src/gd3_document_converter.cpp"
               "src/gd3_exporter.cpp"
               "src/gd3_scene_writer.cpp"
               "src/godot_scene_format.cpp"
               "src/godot_scene_format_plugin.cpp"
               "src/logging.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/godot_tscn/api.hpp"
               "inc/tactile/godot_tscn/gd3_document_converter.hpp"
               "inc/tactile/godot_tscn/gd3_exporter.hpp"
               "inc/tactile/godot_tscn/gd3_scene_writer.hpp"
               "inc/tactile/godot_tscn/gd3_types.hpp"
               "inc/tactile/godot_tscn/godot_scene_format.hpp"
               "inc/tactile/godot_tscn/godot_scene_format_plugin.hpp"
               "inc/tactile/godot_tscn/logging.hpp"
               )

tactile_prepare_target(tactile-godot-tscn)

target_compile_definitions(tactile-godot-tscn
                           PRIVATE
                           "TACTILE_BUILDING_GODOT_TSCN"
                           )

target_link_libraries(tactile-godot-tscn
                      PUBLIC
                      tactile::base
                      tactile::log
                      )
