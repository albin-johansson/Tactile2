project(tactile-zlib-compression-lib CXX)

add_library(tactile-zlib-compression SHARED)
add_library(tactile::zlib_compression ALIAS tactile-zlib-compression)

target_sources(tactile-zlib-compression
               PRIVATE
               "src/logging.cpp"
               "src/zlib_compression_format.cpp"
               "src/zlib_compression_plugin.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/zlib/api.hpp"
               "inc/tactile/zlib/logging.hpp"
               "inc/tactile/zlib/zlib_compression_format.hpp"
               "inc/tactile/zlib/zlib_compression_plugin.hpp"
               )

tactile_prepare_target(tactile-zlib-compression)

target_compile_definitions(tactile-zlib-compression PRIVATE "TACTILE_BUILDING_ZLIB_COMPRESSION")

target_link_libraries(tactile-zlib-compression
                      PUBLIC
                      tactile::base
                      tactile::log

                      PRIVATE
                      ZLIB::ZLIB
                      )
