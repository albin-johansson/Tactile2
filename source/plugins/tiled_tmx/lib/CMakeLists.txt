project(tactile-tiled-tmx-lib CXX)

add_library(tactile-tiled-tmx SHARED)
add_library(tactile::tiled_tmx ALIAS tactile-tiled-tmx)

target_sources(tactile-tiled-tmx
               PRIVATE
               "src/logging.cpp"
               "src/tmx_common.cpp"
               "src/tmx_format_parser.cpp"
               "src/tmx_format_plugin.cpp"
               "src/tmx_format_save_visitor.cpp"
               "src/tmx_save_format.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/tiled_tmx/api.hpp"
               "inc/tactile/tiled_tmx/logging.hpp"
               "inc/tactile/tiled_tmx/tmx_common.hpp"
               "inc/tactile/tiled_tmx/tmx_format_parser.hpp"
               "inc/tactile/tiled_tmx/tmx_format_plugin.hpp"
               "inc/tactile/tiled_tmx/tmx_format_save_visitor.hpp"
               "inc/tactile/tiled_tmx/tmx_save_format.hpp"
               )

tactile_prepare_target(tactile-tiled-tmx)

target_compile_definitions(tactile-tiled-tmx
                           PRIVATE
                           "TACTILE_BUILDING_TILED_TMX_FORMAT"
                           )

target_include_directories(tactile-tiled-tmx
                           PRIVATE
                           "${CPPCODEC_INCLUDE_DIRS}"
                           )

target_link_libraries(tactile-tiled-tmx
                      PUBLIC
                      tactile::base
                      tactile::log
                      pugixml::pugixml
                      )
