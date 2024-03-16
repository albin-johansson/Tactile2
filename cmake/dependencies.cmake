function(tactile_find_dependencies)
  find_package(OpenGL REQUIRED)
  find_package(Boost REQUIRED)
  find_package(argparse CONFIG REQUIRED)
  find_package(EnTT CONFIG REQUIRED)
  find_package(FastFloat CONFIG REQUIRED)
  find_package(fmt CONFIG REQUIRED)
  find_package(GLEW CONFIG REQUIRED)
  find_package(glm CONFIG REQUIRED)
  find_package(imgui CONFIG REQUIRED)
  find_package(magic_enum CONFIG REQUIRED)
  find_package(nlohmann_json CONFIG REQUIRED)
  find_package(Protobuf CONFIG REQUIRED)
  find_package(pugixml CONFIG REQUIRED)
  find_package(SDL2 CONFIG REQUIRED)
  find_package(SDL2_image CONFIG REQUIRED)
  find_package(spdlog CONFIG REQUIRED)
  find_package(tinyfiledialogs CONFIG REQUIRED)
  find_package(tl-expected CONFIG REQUIRED)
  find_package(yaml-cpp CONFIG REQUIRED)
  find_package(zstd CONFIG REQUIRED)
  find_package(ZLIB REQUIRED)

  find_path(CPPCODEC_INCLUDE_DIRS "cppcodec/base32_crockford.hpp")
  find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")

  if (TACTILE_BUILD_TESTS MATCHES "ON")
    find_package(GTest CONFIG REQUIRED)
  endif ()
endfunction()
