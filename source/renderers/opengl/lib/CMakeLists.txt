project(tactile-renderers-opengl-lib CXX)

add_library(tactile-opengl-renderer SHARED)
add_library(tactile::opengl_renderer ALIAS tactile-opengl-renderer)

target_sources(tactile-opengl-renderer
               PRIVATE
               "src/logging.cpp"
               "src/opengl_error.cpp"
               "src/opengl_renderer.cpp"
               "src/opengl_renderer_plugin.cpp"
               "src/opengl_texture.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/opengl/api.hpp"
               "inc/tactile/opengl/logging.hpp"
               "inc/tactile/opengl/opengl_error.hpp"
               "inc/tactile/opengl/opengl_renderer.hpp"
               "inc/tactile/opengl/opengl_renderer_plugin.hpp"
               "inc/tactile/opengl/opengl_texture.hpp"
               )

tactile_prepare_target(tactile-opengl-renderer)

target_compile_definitions(tactile-opengl-renderer
                           PRIVATE
                           "TACTILE_BUILDING_OPENGL_RENDERER"
                           )

target_include_directories(tactile-opengl-renderer
                           SYSTEM PRIVATE
                           "${Stb_INCLUDE_DIR}"
                           )

target_link_libraries(tactile-opengl-renderer
                      PUBLIC
                      tactile::base
                      tactile::log

                      PRIVATE
                      imgui::imgui
                      glad::glad
                      OpenGL::GL
                      SDL2::SDL2
                      )
