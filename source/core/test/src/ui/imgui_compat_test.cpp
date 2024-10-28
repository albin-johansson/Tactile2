// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/ui/imgui_compat.hpp"

#include <gtest/gtest.h>

namespace tactile::core {
namespace {

// tactile::core::to_float2
TEST(ImGuiCompat, ToFloat2)
{
  constexpr ImVec2 input {-10.0f, 20.0f};
  constexpr Float2 output = to_float2(input);

  EXPECT_EQ(output.x(), input.x);
  EXPECT_EQ(output.y(), input.y);
}

// tactile::core::to_float4
TEST(ImGuiCompat, ToFloat4)
{
  constexpr ImVec4 input {1.0f, 2.0f, 3.0f, 4.0f};
  constexpr Float4 output = to_float4(input);

  EXPECT_EQ(output.x(), input.x);
  EXPECT_EQ(output.y(), input.y);
  EXPECT_EQ(output.z(), input.z);
  EXPECT_EQ(output.w(), input.w);
}

// tactile::core::to_int2
TEST(ImGuiCompat, ToInt2)
{
  constexpr ImVec2 input {-3.4f, 9.7f};
  constexpr Int2 output = to_int2(input);

  EXPECT_EQ(output.x(), static_cast<Int4::value_type>(input.x));
  EXPECT_EQ(output.y(), static_cast<Int4::value_type>(input.y));
}

// tactile::core::to_int4
TEST(ImGuiCompat, ToInt4)
{
  constexpr ImVec4 input {1.5f, 2.1f, 3.6f, 4.9f};
  constexpr Int4 output = to_int4(input);

  EXPECT_EQ(output.x(), static_cast<Int4::value_type>(input.x));
  EXPECT_EQ(output.y(), static_cast<Int4::value_type>(input.y));
  EXPECT_EQ(output.z(), static_cast<Int4::value_type>(input.z));
  EXPECT_EQ(output.w(), static_cast<Int4::value_type>(input.w));
}

// tactile::core::to_imgui_data_type
TEST(ImGuiCompat, ToImGuiDataType)
{
  EXPECT_EQ(to_imgui_data_type<float>(), ImGuiDataType_Float);
  EXPECT_EQ(to_imgui_data_type<double>(), ImGuiDataType_Double);

  EXPECT_EQ(to_imgui_data_type<std::uint8_t>(), ImGuiDataType_U8);
  EXPECT_EQ(to_imgui_data_type<std::uint16_t>(), ImGuiDataType_U16);
  EXPECT_EQ(to_imgui_data_type<std::uint32_t>(), ImGuiDataType_U32);
  EXPECT_EQ(to_imgui_data_type<std::uint64_t>(), ImGuiDataType_U64);

  EXPECT_EQ(to_imgui_data_type<std::int8_t>(), ImGuiDataType_S8);
  EXPECT_EQ(to_imgui_data_type<std::int16_t>(), ImGuiDataType_S16);
  EXPECT_EQ(to_imgui_data_type<std::int32_t>(), ImGuiDataType_S32);
  EXPECT_EQ(to_imgui_data_type<std::int64_t>(), ImGuiDataType_S64);
}

}  // namespace
}  // namespace tactile::core
