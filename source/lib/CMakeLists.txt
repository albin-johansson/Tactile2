project(tactile-lib CXX)

file(GLOB_RECURSE SOURCE_FILES
     CONFIGURE_DEPENDS
     ${PROJECT_SOURCE_DIR}/**/*.cpp
     ${PROJECT_SOURCE_DIR}/**/*.hpp
     )

add_library(libtactile ${SOURCE_FILES})
add_library(tactile::lib ALIAS libtactile)

tactile_prepare_target(libtactile)

target_include_directories(libtactile
                           PUBLIC
                           ${PROJECT_SOURCE_DIR}

                           SYSTEM PUBLIC
                           ${TACTILE_VENDOR_DIR}/centurion
                           ${TACTILE_VENDOR_DIR}/fontawesome
                           ${STB_INCLUDE_DIRS}
                           ${TACTILE_PROTO_GENERATED_DIR}
                           ${Boost_INCLUDE_DIRS}
                           ${CPPCODEC_INCLUDE_DIRS}
                           )

target_link_libraries(libtactile
                      PUBLIC
                      tactile::proto
                      tactile::base
                      tactile::core
                      tactile::opengl

                      Boost::boost
                      argparse::argparse
                      SDL2::SDL2
                      SDL2_image::SDL2_image
                      imgui::imgui
                      EnTT::EnTT
                      magic_enum::magic_enum
                      fmt::fmt
                      protobuf::libprotobuf
                      yaml-cpp::yaml-cpp
                      nlohmann_json::nlohmann_json
                      pugixml::pugixml
                      tl::expected
                      glad::glad
                      ZLIB::ZLIB
                      spdlog::spdlog
                      zstd::libzstd_static
                      )

target_compile_definitions(libtactile
                           PUBLIC
                           CENTURION_NO_SDL_MIXER
                           CENTURION_NO_SDL_TTF
                           IMGUI_DEFINE_MATH_OPERATORS
                           GLEW_NO_GLU
                           BOOST_ENABLE_ASSERT_DEBUG_HANDLER
                           BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
                           BOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT
                           BOOST_NO_CXX98_FUNCTION_BASE
                           ENTT_STANDARD_CPP
                           SPDLOG_FMT_EXTERNAL
                           )

if (MSVC)
  target_compile_definitions(libtactile
                             PUBLIC
                             WIN32_LEAN_AND_MEAN
                             NOMINMAX
                             )
endif ()

if (TACTILE_BUILD_APP_BUNDLE MATCHES ON)
  target_compile_definitions(libtactile PUBLIC TACTILE_BUILD_APP_BUNDLE)
endif ()
