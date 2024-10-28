// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/meta/attribute_type.hpp"

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::parse_attribute_type
TEST(AttributeType, ParseAttributeType)
{
  EXPECT_EQ(parse_attribute_type("string"), AttributeType::kStr);
  EXPECT_EQ(parse_attribute_type("int"), AttributeType::kInt);
  EXPECT_EQ(parse_attribute_type("int2"), AttributeType::kInt2);
  EXPECT_EQ(parse_attribute_type("int3"), AttributeType::kInt3);
  EXPECT_EQ(parse_attribute_type("int4"), AttributeType::kInt4);
  EXPECT_EQ(parse_attribute_type("float"), AttributeType::kFloat);
  EXPECT_EQ(parse_attribute_type("float2"), AttributeType::kFloat2);
  EXPECT_EQ(parse_attribute_type("float3"), AttributeType::kFloat3);
  EXPECT_EQ(parse_attribute_type("float4"), AttributeType::kFloat4);
  EXPECT_EQ(parse_attribute_type("bool"), AttributeType::kBool);
  EXPECT_EQ(parse_attribute_type("file"), AttributeType::kPath);
  EXPECT_EQ(parse_attribute_type("path"), AttributeType::kPath);
  EXPECT_EQ(parse_attribute_type("color"), AttributeType::kColor);
  EXPECT_EQ(parse_attribute_type("object"), AttributeType::kObject);

  EXPECT_FALSE(parse_attribute_type("foo").has_value());
  EXPECT_FALSE(parse_attribute_type("int ").has_value());
}

// tactile::to_string [AttributeType]
TEST(AttributeType, ToString)
{
  EXPECT_EQ(to_string(AttributeType::kStr), "string");
  EXPECT_EQ(to_string(AttributeType::kInt), "int");
  EXPECT_EQ(to_string(AttributeType::kInt2), "int2");
  EXPECT_EQ(to_string(AttributeType::kInt3), "int3");
  EXPECT_EQ(to_string(AttributeType::kInt4), "int4");
  EXPECT_EQ(to_string(AttributeType::kFloat), "float");
  EXPECT_EQ(to_string(AttributeType::kFloat2), "float2");
  EXPECT_EQ(to_string(AttributeType::kFloat3), "float3");
  EXPECT_EQ(to_string(AttributeType::kFloat4), "float4");
  EXPECT_EQ(to_string(AttributeType::kBool), "bool");
  EXPECT_EQ(to_string(AttributeType::kPath), "path");
  EXPECT_EQ(to_string(AttributeType::kColor), "color");
  EXPECT_EQ(to_string(AttributeType::kObject), "object");
}

}  // namespace
}  // namespace tactile
