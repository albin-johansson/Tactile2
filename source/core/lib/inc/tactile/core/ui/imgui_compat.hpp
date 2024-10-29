// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <concepts>   // signed_integral, unsigned_integral, floating_point
#include <stdexcept>  // runtime_error

#include <imgui.h>

#include "tactile/base/numeric/vec.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/util/concepts.hpp"

namespace tactile::core {

/**
 * Creates a Tactile vector from an ImGui vector.
 *
 * \param vec The vector to convert.
 *
 * \return
 * A Tactile vector.
 */
[[nodiscard]]
constexpr auto to_float2(const ImVec2& vec) noexcept -> Float2
{
  return Float2 {vec.x, vec.y};
}

/**
 * \copydoc to_float2
 */
[[nodiscard]]
constexpr auto to_float4(const ImVec4& vec) noexcept -> Float4
{
  return Float4 {vec.x, vec.y, vec.z, vec.w};
}

/**
 * \copydoc to_float2
 */
[[nodiscard]]
constexpr auto to_int2(const ImVec2& vec) noexcept -> Int2
{
  return Int2 {static_cast<Int2::value_type>(vec.x), static_cast<Int2::value_type>(vec.y)};
}

/**
 * \copydoc to_float2
 */
[[nodiscard]]
constexpr auto to_int4(const ImVec4& vec) noexcept -> Int4
{
  return Int4 {static_cast<Int2::value_type>(vec.x),
               static_cast<Int2::value_type>(vec.y),
               static_cast<Int2::value_type>(vec.z),
               static_cast<Int2::value_type>(vec.w)};
}

/**
 * Creates an ImGui vector from a Tactile vector.
 *
 * \param vec The vector to convert.
 *
 * \return
 * An ImGui vector.
 */
[[nodiscard]]
constexpr auto to_imvec2(const Float2& vec) noexcept -> ImVec2
{
  return ImVec2 {vec.x(), vec.y()};
}

/**
 * \copydoc to_imvec2
 */
[[nodiscard]]
constexpr auto to_imvec2(const Int2& vec) noexcept -> ImVec2
{
  return ImVec2 {static_cast<float>(vec.x()), static_cast<float>(vec.y())};
}

/**
 * \copydoc to_imvec2
 */
[[nodiscard]]
constexpr auto to_imvec4(const Float4& vec) noexcept -> ImVec4
{
  return ImVec4 {vec.x(), vec.y(), vec.z(), vec.w()};
}

/**
 * \copydoc to_imvec2
 */
[[nodiscard]]
constexpr auto to_imvec4(const Int4& vec) noexcept -> ImVec4
{
  return ImVec4 {static_cast<float>(vec.x()),
                 static_cast<float>(vec.y()),
                 static_cast<float>(vec.z()),
                 static_cast<float>(vec.w())};
}

/**
 * Returns the ImGui data type identifier for a given type.
 *
 * \tparam T A signed integer type.
 *
 * \return
 * An ImGui data type identifier.
 */
template <std::signed_integral T>
[[nodiscard]] consteval auto to_imgui_data_type() -> ImGuiDataType
{
  switch (sizeof(T)) {
    case sizeof(std::int8_t):  return ImGuiDataType_S8;
    case sizeof(std::int16_t): return ImGuiDataType_S16;
    case sizeof(std::int32_t): return ImGuiDataType_S32;
    case sizeof(std::int64_t): return ImGuiDataType_S64;
    default:                   throw std::runtime_error {"unsupported signed integer size"};
  }
}

/**
 * Returns the ImGui data type identifier for a given type.
 *
 * \tparam T An unsigned integer type.
 *
 * \return
 * An ImGui data type identifier.
 */
template <std::unsigned_integral T>
[[nodiscard]] consteval auto to_imgui_data_type() -> ImGuiDataType
{
  switch (sizeof(T)) {
    case sizeof(std::uint8_t):  return ImGuiDataType_U8;
    case sizeof(std::uint16_t): return ImGuiDataType_U16;
    case sizeof(std::uint32_t): return ImGuiDataType_U32;
    case sizeof(std::uint64_t): return ImGuiDataType_U64;
    default:                    throw std::runtime_error {"unsupported unsigned integer size"};
  }
}

/**
 * Returns the ImGui data type identifier for a given type.
 *
 * \tparam T A floating-point type.
 *
 * \return
 * An ImGui data type identifier.
 */
template <std::floating_point T>
[[nodiscard]] consteval auto to_imgui_data_type() -> ImGuiDataType
{
  switch (sizeof(T)) {
    case sizeof(float):  return ImGuiDataType_Float;
    case sizeof(double): return ImGuiDataType_Double;
    default:             throw std::runtime_error {"unsupported float size"};
  }
}

}  // namespace tactile::core
