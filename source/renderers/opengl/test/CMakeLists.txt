project(tactile-renderers-opengl-test CXX)

add_executable(tactile-opengl-renderer-test)

target_sources(tactile-opengl-renderer-test
               PRIVATE
               "src/main.cpp"
               "src/opengl_error_test.cpp"
               "src/opengl_renderer_test.cpp"
               "src/opengl_texture_test.cpp"
               )

tactile_prepare_target(tactile-opengl-renderer-test)

target_link_libraries(tactile-opengl-renderer-test
                      PUBLIC
                      tactile::opengl_renderer
                      GTest::gtest

                      PRIVATE
                      tactile::runtime
                      glad::glad
                      SDL2::SDL2
                      imgui::imgui
                      )
