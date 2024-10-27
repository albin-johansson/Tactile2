project(tactile-renderers-null-lib CXX)

add_library(tactile-null-renderer SHARED)
add_library(tactile::null_renderer ALIAS tactile-null-renderer)

target_sources(tactile-null-renderer
               PRIVATE
               "src/null_renderer.cpp"
               "src/null_renderer_plugin.cpp"
               "src/null_texture.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/null_renderer/api.hpp"
               "inc/tactile/null_renderer/null_renderer.hpp"
               "inc/tactile/null_renderer/null_renderer_plugin.hpp"
               "inc/tactile/null_renderer/null_texture.hpp"
               )

tactile_prepare_target(tactile-null-renderer)

target_compile_definitions(tactile-null-renderer
                           PRIVATE
                           "TACTILE_BUILDING_NULL_RENDERER"
                           )

target_include_directories(tactile-null-renderer
                           SYSTEM PRIVATE
                           "${Stb_INCLUDE_DIR}"
                           )

target_link_libraries(tactile-null-renderer
                      PUBLIC
                      tactile::base
                      )
