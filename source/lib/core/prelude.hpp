/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2023 Albin Johansson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>  // size_t, ptrdiff_t, uintptr_t
#include <cstdint>  // int{}_t, uint{}_t

#include <boost/config.hpp>
#include <boost/predef/compiler.h>
#include <boost/predef/os.h>

#define TACTILE_FWD_DECLARE_CLASS(Name) class Name;
#define TACTILE_FWD_DECLARE_STRUCT(Name) struct Name;

#define TACTILE_FWD_DECLARE_CLASS_NS(Ns, Name) \
  namespace Ns {                               \
  class Name;                                  \
  }

#define TACTILE_FWD_DECLARE_STRUCT_NS(Ns, Name) \
  namespace Ns {                                \
  struct Name;                                  \
  }

#define TACTILE_DEFAULT_COPY(Class) \
  Class(const Class&) = default;    \
  auto operator=(const Class&) -> Class& = default

#define TACTILE_DEFAULT_MOVE(Class)  \
  Class(Class&&) noexcept = default; \
  auto operator=(Class&&) noexcept -> Class& = default

#define TACTILE_DELETE_COPY(Class) \
  Class(const Class&) = delete;    \
  auto operator=(const Class&) -> Class& = delete

#define TACTILE_DELETE_MOVE(Class)  \
  Class(Class&&) noexcept = delete; \
  auto operator=(Class&&) noexcept -> Class& = delete

#define TACTILE_DECLARE_COPY(Class) \
  Class(const Class&);              \
  Class& operator=(const Class&)

#define TACTILE_DECLARE_MOVE(Class) \
  Class(Class&&) noexcept;          \
  Class& operator=(Class&&) noexcept

#define TACTILE_DEFINE_COPY(Class)      \
  Class::Class(const Class&) = default; \
  Class& Class::operator=(const Class&) = default

#define TACTILE_DEFINE_MOVE(Class)          \
  Class::Class(Class&&) noexcept = default; \
  Class& Class::operator=(Class&&) noexcept = default

#define TACTILE_DECLARE_SPECIAL_MEMBERS(Class) \
  TACTILE_DECLARE_COPY(Class);                 \
  TACTILE_DECLARE_MOVE(Class);                 \
  ~Class()

#define TACTILE_VERSION_STRING "0.5.0 [WIP]"

// Build mode
#ifdef NDEBUG
#define TACTILE_DEBUG 0
#define TACTILE_RELEASE 1
#else
#define TACTILE_DEBUG 1
#define TACTILE_RELEASE 0
#endif  // NDEBUG

// Compiler detection
#define TACTILE_COMPILER_MSVC BOOST_COMP_MSVC
#define TACTILE_COMPILER_CLANG BOOST_COMP_CLANG
#define TACTILE_COMPILER_GCC BOOST_COMP_GNUC

// OS detection
#define TACTILE_PLATFORM_WINDOWS BOOST_OS_WINDOWS
#define TACTILE_OS_WINDOWS BOOST_OS_WINDOWS
#define TACTILE_OS_LINUX BOOST_OS_LINUX
#define TACTILE_OS_MACOS BOOST_OS_MACOS

// Attributes
#define TACTILE_NOINLINE BOOST_NOINLINE

namespace tactile {

using uchar = unsigned char;
using uint = unsigned int;
using ulonglong = unsigned long long int;

using usize = std::size_t;
using ssize = std::ptrdiff_t;
using uintptr = std::uintptr_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using float32 = float;
using float64 = double;

static_assert(sizeof(float32) == 4);
static_assert(sizeof(float64) == 8);

using LayerID = int32;
using ObjectID = int32;
using TileIndex = int32;  ///< For local tile identifiers.
using TileID = int32;     ///< For global tile identifiers.

inline constexpr TileID kEmptyTile = 0;

#if TACTILE_DEBUG
inline constexpr bool kIsDebugBuild = true;
inline constexpr bool kIsReleaseBuild = false;
#else
inline constexpr bool kIsDebugBuild = false;
inline constexpr bool kIsReleaseBuild = true;
#endif  // TACTILE_DEBUG

#if TACTILE_OS_WINDOWS
inline constexpr bool kOnWindows = true;
#else
inline constexpr bool kOnWindows = false;
#endif  // TACTILE_OS_WINDOWS

#if TACTILE_OS_MACOS
inline constexpr bool kOnMacos = true;
#else
inline constexpr bool kOnMacos = false;
#endif  // TACTILE_OS_MACOS

#if TACTILE_OS_LINUX
inline constexpr bool kOnLinux = true;
#else
inline constexpr bool kOnLinux = false;
#endif  // TACTILE_OS_LINUX

#ifdef TACTILE_BUILD_APP_BUNDLE
inline constexpr bool kIsAppBundle = true;
#else
inline constexpr bool kIsAppBundle = false;
#endif  // TACTILE_BUILD_APP_BUNDLE

}  // namespace tactile