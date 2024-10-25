project(tactile-plugins-tiled-tmj-lib CXX)

add_library(tactile-tiled-tmj SHARED)
add_library(tactile::tiled_tmj ALIAS tactile-tiled-tmj)

target_sources(tactile-tiled-tmj
               PRIVATE
               "src/tmj_common.cpp"
               "src/tmj_format_parser.cpp"
               "src/tmj_format_plugin.cpp"
               "src/tmj_format_save_visitor.cpp"
               "src/tmj_save_format.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/tiled_tmj/api.hpp"
               "inc/tactile/tiled_tmj/tmj_common.hpp"
               "inc/tactile/tiled_tmj/tmj_format_parser.hpp"
               "inc/tactile/tiled_tmj/tmj_format_plugin.hpp"
               "inc/tactile/tiled_tmj/tmj_format_save_visitor.hpp"
               "inc/tactile/tiled_tmj/tmj_save_format.hpp"
               )

tactile_prepare_target(tactile-tiled-tmj)

target_compile_definitions(tactile-tiled-tmj
                           PRIVATE
                           "TACTILE_BUILDING_TILED_TMJ"
                           )

target_include_directories(tactile-tiled-tmj
                           PRIVATE
                           "${CPPCODEC_INCLUDE_DIRS}"
                           )

target_link_libraries(tactile-tiled-tmj
                      PUBLIC
                      tactile::base
                      tactile::runtime
                      tactile::json_util
                      nlohmann_json::nlohmann_json
                      )
