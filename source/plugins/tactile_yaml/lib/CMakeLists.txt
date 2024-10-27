project(tactile-yaml-format-lib CXX)

add_library(tactile-yaml-format SHARED)
add_library(tactile::yaml_format ALIAS tactile-yaml-format)

target_sources(tactile-yaml-format
               PRIVATE
               "src/logging.cpp"
               "src/yaml_format_plugin.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/yaml/api.hpp"
               "inc/tactile/yaml/logging.hpp"
               "inc/tactile/yaml/yaml_format_plugin.hpp"
               )

tactile_prepare_target(tactile-yaml-format)

target_compile_definitions(tactile-yaml-format
                           PRIVATE
                           "TACTILE_BUILDING_YAML_FORMAT"
                           )

target_link_libraries(tactile-yaml-format
                      PUBLIC
                      tactile::base
                      tactile::log

                      PRIVATE
                      yaml-cpp::yaml-cpp
                      )

