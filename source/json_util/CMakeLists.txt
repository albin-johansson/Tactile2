project(tactile-json-util CXX)

find_package(nlohmann_json CONFIG REQUIRED)

add_library(tactile-json-util INTERFACE)
add_library(tactile::json_util ALIAS tactile-json-util)

target_sources(tactile-json-util
               INTERFACE FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/json_util/json_io.hpp"
               )

target_link_libraries(tactile-json-util
                      INTERFACE
                      tactile::base
                      nlohmann_json::nlohmann_json
                      )
