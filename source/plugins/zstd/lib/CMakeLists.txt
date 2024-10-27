project(tactile-plugins-zstd-lib CXX)

add_library(tactile-zstd-compression SHARED)
add_library(tactile::zstd_compression ALIAS tactile-zstd-compression)

target_sources(tactile-zstd-compression
               PRIVATE
               "src/logging.cpp"
               "src/zstd_compression_format.cpp"
               "src/zstd_compression_plugin.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/zstd/api.hpp"
               "inc/tactile/zstd/logging.hpp"
               "inc/tactile/zstd/zstd_compression_format.hpp"
               "inc/tactile/zstd/zstd_compression_plugin.hpp"
               )

tactile_prepare_target(tactile-zstd-compression)

target_compile_definitions(tactile-zstd-compression
                           PRIVATE
                           "TACTILE_BUILDING_ZSTD_COMPRESSION"
                           )

target_link_libraries(tactile-zstd-compression
                      PUBLIC
                      tactile::base
                      tactile::log

                      PRIVATE
                      zstd::libzstd
                      )
