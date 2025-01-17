project(tactile-base-test CXX)

add_executable(tactile-base-test)

target_sources(tactile-base-test
               PRIVATE
               "src/container/lookup_test.cpp"
               "src/container/string_test.cpp"
               "src/io/int_parser_test.cpp"
               "src/io/tile_io_test.cpp"
               "src/meta/attribute_test.cpp"
               "src/meta/attribute_type_test.cpp"
               "src/meta/color_test.cpp"
               "src/numeric/conversion_test.cpp"
               "src/numeric/extent_2d_test.cpp"
               "src/numeric/index_2d_test.cpp"
               "src/numeric/offset_2d_test.cpp"
               "src/numeric/saturate_cast_test.cpp"
               "src/numeric/vec_common_test.cpp"
               "src/numeric/vec_format_test.cpp"
               "src/numeric/vec_stream_test.cpp"
               "src/numeric/vec_test.cpp"
               "src/platform/bits_test.cpp"
               "src/platform/filesystem_test.cpp"
               "src/util/buffer_test.cpp"
               "src/util/format_test.cpp"
               "src/util/scope_exit_test.cpp"
               "src/util/tile_matrix_test.cpp"
               "src/main.cpp"
               )

tactile_prepare_target(tactile-base-test)

target_link_libraries(tactile-base-test
                      PRIVATE
                      tactile::base
                      GTest::gtest
                      )
