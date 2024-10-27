cmake_minimum_required(VERSION 3.16)

option(TACTILE_BUILD_TESTS "Build test suites" OFF)
option(TACTILE_BUILD_YAML_FORMAT "Build with Tactile YAML save format support" ON)
option(TACTILE_BUILD_TILED_TMJ_FORMAT "Build with Tiled TMJ save format support" ON)
option(TACTILE_BUILD_TILED_TMX_FORMAT "Build with Tiled TMX save format support" ON)
option(TACTILE_BUILD_GODOT_TSCN_FORMAT "Build with Godot TSCN save format support" ON)
option(TACTILE_BUILD_ZLIB_COMPRESSION "Build with Zlib compression support" ON)
option(TACTILE_BUILD_ZSTD_COMPRESSION "Build with Zstd compression support" ON)
option(TACTILE_BUILD_OPENGL_RENDERER "Build the OpenGL renderer" OFF)
option(TACTILE_BUILD_VULKAN_RENDERER "Build the Vulkan renderer" OFF)
option(TACTILE_MACOS_APP_BUNDLE "Build the editor as a macOS application bundle (.app)" OFF)
option(TACTILE_USE_LTO "Enable link-time optimizations" OFF)
option(TACTILE_USE_PRECOMPILED_HEADERS "Enable precompiled standard headers" ON)

if (CMAKE_SKIP_INSTALL_ALL_DEPENDENCY MATCHES "")
  set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY "ON")
endif ()

message(DEBUG "TACTILE_BUILD_TESTS: ${TACTILE_BUILD_TESTS}")
message(DEBUG "TACTILE_BUILD_YAML_FORMAT: ${TACTILE_BUILD_YAML_FORMAT}")
message(DEBUG "TACTILE_BUILD_TILED_TMJ_FORMAT: ${TACTILE_BUILD_TILED_TMJ_FORMAT}")
message(DEBUG "TACTILE_BUILD_TILED_TMX_FORMAT: ${TACTILE_BUILD_TILED_TMX_FORMAT}")
message(DEBUG "TACTILE_BUILD_GODOT_TSCN_FORMAT: ${TACTILE_BUILD_GODOT_TSCN_FORMAT}")
message(DEBUG "TACTILE_BUILD_ZLIB_COMPRESSION: ${TACTILE_BUILD_ZLIB_COMPRESSION}")
message(DEBUG "TACTILE_BUILD_ZSTD_COMPRESSION: ${TACTILE_BUILD_ZSTD_COMPRESSION}")
message(DEBUG "TACTILE_BUILD_OPENGL_RENDERER: ${TACTILE_BUILD_OPENGL_RENDERER}")
message(DEBUG "TACTILE_BUILD_VULKAN_RENDERER: ${TACTILE_BUILD_VULKAN_RENDERER}")
message(DEBUG "TACTILE_MACOS_APP_BUNDLE: ${TACTILE_MACOS_APP_BUNDLE}")
message(DEBUG "TACTILE_USE_LTO: ${TACTILE_USE_LTO}")
message(DEBUG "TACTILE_USE_PRECOMPILED_HEADERS: ${TACTILE_USE_PRECOMPILED_HEADERS}")

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  cmake_path(SET VCPKG_ROOT NORMALIZE "$ENV{VCPKG_ROOT}")
  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif ()

if (TACTILE_BUILD_OPENGL_RENDERER)
  list(APPEND VCPKG_MANIFEST_FEATURES "opengl")
endif ()

if (TACTILE_BUILD_VULKAN_RENDERER)
  list(APPEND VCPKG_MANIFEST_FEATURES "vulkan")
endif ()

message(DEBUG "CMAKE_SKIP_INSTALL_ALL_DEPENDENCY: ${CMAKE_SKIP_INSTALL_ALL_DEPENDENCY}")
message(DEBUG "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(DEBUG "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
message(DEBUG "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")

project(tactile
        HOMEPAGE_URL "https://github.com/albin-johansson/tactile"
        VERSION 0.5.0
        LANGUAGES CXX)

# Determine build type, e.g. "debug" or "release".
string(TOLOWER "${CMAKE_BUILD_TYPE}" TACTILE_BUILD_TYPE)
if (NOT (TACTILE_BUILD_TYPE MATCHES "debug|release|asan"))
  message(FATAL_ERROR "Unsupported build type")
else ()
  message(DEBUG "TACTILE_BUILD_TYPE: ${TACTILE_BUILD_TYPE}")
endif ()

set(TACTILE_ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(TACTILE_BUILD_DIR "${TACTILE_ROOT_DIR}/build/${TACTILE_BUILD_TYPE}")
set(TACTILE_VENDOR_DIR "${TACTILE_ROOT_DIR}/vendor")
set(TACTILE_ASSET_DIR "${TACTILE_ROOT_DIR}/assets")

message(DEBUG "TACTILE_BUILD_DIR: ${TACTILE_BUILD_DIR}")
message(DEBUG "TACTILE_ROOT_DIR: ${TACTILE_ROOT_DIR}")
message(DEBUG "TACTILE_VENDOR_DIR: ${TACTILE_VENDOR_DIR}")
message(DEBUG "TACTILE_ASSET_DIR: ${TACTILE_ASSET_DIR}")

install(DIRECTORY "${TACTILE_ASSET_DIR}" DESTINATION "${TACTILE_BUILD_DIR}")

include("${TACTILE_ROOT_DIR}/cmake/tactile.cmake")

if (TACTILE_BUILD_TESTS)
  find_package(GTest CONFIG REQUIRED)
endif ()

add_subdirectory("source/proto")
add_subdirectory("source/base")

if (TACTILE_BUILD_TESTS)
  add_subdirectory("source/test_util")
endif ()

add_subdirectory("source/log")
add_subdirectory("source/core")
add_subdirectory("source/runtime")
add_subdirectory("source/json_util")
add_subdirectory("source/main")

if (TACTILE_BUILD_YAML_FORMAT)
  add_subdirectory("source/plugins/tactile_yaml")
endif ()

if (TACTILE_BUILD_TILED_TMJ_FORMAT)
  add_subdirectory("source/plugins/tiled_tmj")
endif ()

if (TACTILE_BUILD_TILED_TMX_FORMAT)
  add_subdirectory("source/plugins/tiled_tmx")
endif ()

if (TACTILE_BUILD_GODOT_TSCN_FORMAT)
  add_subdirectory("source/plugins/godot_tscn")
endif ()

if (TACTILE_BUILD_ZLIB_COMPRESSION)
  add_subdirectory("source/plugins/zlib")
endif ()

if (TACTILE_BUILD_ZSTD_COMPRESSION)
  add_subdirectory("source/plugins/zstd")
endif ()

add_subdirectory("source/renderers/null")

if (TACTILE_BUILD_OPENGL_RENDERER)
  add_subdirectory("source/renderers/opengl")
endif ()

if (TACTILE_BUILD_VULKAN_RENDERER)
  add_subdirectory("source/renderers/vulkan")
endif ()
