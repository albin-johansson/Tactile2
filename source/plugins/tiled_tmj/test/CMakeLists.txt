project(tactile-plugins-tiled-tmj-test CXX)

add_executable(tactile-tiled-tmj-test)

target_sources(tactile-tiled-tmj-test
               PRIVATE
               "src/main.cpp"
               )

tactile_prepare_target(tactile-tiled-tmj-test)

target_link_libraries(tactile-tiled-tmj-test
                      PRIVATE
                      tactile::tiled_tmj
                      tactile::test_util
                      tactile::null_renderer
                      $<$<BOOL:TACTILE_BUILD_ZLIB_COMPRESSION>:tactile::zlib_compression>
                      $<$<BOOL:TACTILE_BUILD_ZSTD_COMPRESSION>:tactile::zstd_compression>
                      GTest::gtest
                      )

target_compile_definitions(tactile-tiled-tmj-test
                           PRIVATE
                           "$<$<BOOL:TACTILE_BUILD_ZLIB_COMPRESSION>:TACTILE_HAS_ZLIB_COMPRESSION>"
                           "$<$<BOOL:TACTILE_BUILD_ZSTD_COMPRESSION>:TACTILE_HAS_ZSTD_COMPRESSION>"
                           )
