project(tactile-runtime-test CXX)

add_executable(tactile-runtime-test)

target_sources(tactile-runtime-test
               PRIVATE
               "src/main.cpp"
               "src/save_format_roundtrip_test.cpp"
               )

tactile_prepare_target(tactile-runtime-test)

target_link_libraries(tactile-runtime-test
                      PRIVATE
                      tactile::test_util
                      tactile::null_renderer
                      $<$<BOOL:TACTILE_BUILD_TILED_TMJ_FORMAT>:tactile::tiled_tmj>
                      $<$<BOOL:TACTILE_BUILD_TILED_TMX_FORMAT>:tactile::tiled_tmx>
                      $<$<BOOL:TACTILE_BUILD_ZLIB_COMPRESSION>:tactile::zlib_compression>
                      $<$<BOOL:TACTILE_BUILD_ZSTD_COMPRESSION>:tactile::zstd_compression>
                      GTest::gtest
                      )

target_compile_definitions(tactile-runtime-test
                           PRIVATE
                           "$<$<BOOL:TACTILE_BUILD_TILED_TMJ_FORMAT>:TACTILE_HAS_TILED_TMJ>"
                           "$<$<BOOL:TACTILE_BUILD_TILED_TMX_FORMAT>:TACTILE_HAS_TILED_TMX>"
                           "$<$<BOOL:TACTILE_BUILD_ZLIB_COMPRESSION>:TACTILE_HAS_ZLIB>"
                           "$<$<BOOL:TACTILE_BUILD_ZSTD_COMPRESSION>:TACTILE_HAS_ZSTD>"
                           )
