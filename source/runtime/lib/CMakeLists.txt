project(tactile-runtime-lib CXX)

add_library(tactile-runtime SHARED)
add_library(tactile::runtime ALIAS tactile-runtime)

target_sources(tactile-runtime
               PRIVATE
               "src/command_line_options.cpp"
               "src/document_factory.cpp"
               "src/dynamic_library.cpp"
               "src/launcher.cpp"
               "src/logging.cpp"
               "src/plugin_instance.cpp"
               "src/protobuf_context.cpp"
               "src/runtime_impl.cpp"
               "src/sdl_context.cpp"
               "src/window_impl.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/runtime/api.hpp"
               "inc/tactile/runtime/command_line_options.hpp"
               "inc/tactile/runtime/document_factory.hpp"
               "inc/tactile/runtime/dynamic_library.hpp"
               "inc/tactile/runtime/launcher.hpp"
               "inc/tactile/runtime/logging.hpp"
               "inc/tactile/runtime/plugin_instance.hpp"
               "inc/tactile/runtime/protobuf_context.hpp"
               "inc/tactile/runtime/runtime_impl.hpp"
               "inc/tactile/runtime/sdl_context.hpp"
               "inc/tactile/runtime/window_impl.hpp"
               )

tactile_prepare_target(tactile-runtime)

target_compile_definitions(tactile-runtime
                           PRIVATE
                           "TACTILE_BUILDING_RUNTIME"
                           )

target_link_libraries(tactile-runtime
                      PUBLIC
                      tactile::base
                      tactile::log

                      PRIVATE
                      tactile::core
                      SDL2::SDL2
                      argparse::argparse
                      )
