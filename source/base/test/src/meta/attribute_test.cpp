// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/meta/attribute.hpp"

#include <string>  // string

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::Attribute::Attribute
TEST(Attribute, DefaultConstructor)
{
  const Attribute attribute {};
  EXPECT_EQ(attribute.get_type(), AttributeType::kStr);
  EXPECT_TRUE(attribute.has_default_value());
}

// tactile::Attribute::Attribute [AttributeType]
TEST(Attribute, ExplicitTypeConstructor)
{
  EXPECT_EQ(Attribute {AttributeType::kStr}, Attribute {std::string {}});
  EXPECT_EQ(Attribute {AttributeType::kInt}, Attribute {std::int32_t {}});
  EXPECT_EQ(Attribute {AttributeType::kInt2}, Attribute {Int2 {}});
  EXPECT_EQ(Attribute {AttributeType::kInt3}, Attribute {Int3 {}});
  EXPECT_EQ(Attribute {AttributeType::kInt4}, Attribute {Int4 {}});
  EXPECT_EQ(Attribute {AttributeType::kFloat}, Attribute {float {}});
  EXPECT_EQ(Attribute {AttributeType::kFloat2}, Attribute {Float2 {}});
  EXPECT_EQ(Attribute {AttributeType::kFloat3}, Attribute {Float3 {}});
  EXPECT_EQ(Attribute {AttributeType::kFloat4}, Attribute {Float4 {}});
  EXPECT_EQ(Attribute {AttributeType::kBool}, Attribute {bool {}});
  EXPECT_EQ(Attribute {AttributeType::kColor}, Attribute {UColor {}});
  EXPECT_EQ(Attribute {AttributeType::kPath}, Attribute {std::filesystem::path {}});
  EXPECT_EQ(Attribute {AttributeType::kObject}, Attribute {ObjectRef {}});
}

// tactile::Attribute::Attribute [T]
TEST(Attribute, ImplicitValueConstructor)
{
  {
    const Attribute str1 {"foo"};
    const Attribute str2 {std::string {"bar"}};

    EXPECT_EQ(str1.get_type(), AttributeType::kStr);
    EXPECT_EQ(str2.get_type(), AttributeType::kStr);

    EXPECT_EQ(str1.as_string(), "foo");
    EXPECT_EQ(str2.as_string(), "bar");
  }

  {
    const Attribute integer {42};
    EXPECT_EQ(integer.get_type(), AttributeType::kInt);
    EXPECT_EQ(integer.as_int(), 42);
  }

  {
    const Attribute real {1.5f};
    EXPECT_EQ(real.get_type(), AttributeType::kFloat);
    EXPECT_EQ(real.as_float(), 1.5f);
  }

  {
    const Attribute t {true};
    const Attribute f {false};

    EXPECT_EQ(t.get_type(), AttributeType::kBool);
    EXPECT_EQ(f.get_type(), AttributeType::kBool);

    EXPECT_TRUE(t.as_bool());
    EXPECT_FALSE(f.as_bool());
  }

  {
    const Attribute color {UColor {1, 2, 3, 4}};
    EXPECT_EQ(color.get_type(), AttributeType::kColor);
    EXPECT_EQ(color.as_color(), (UColor {1, 2, 3, 4}));
  }

  {
    const Attribute path {std::filesystem::path {"foo/bar"}};
    EXPECT_EQ(path.get_type(), AttributeType::kPath);
    EXPECT_EQ(path.as_path(), "foo/bar");
  }

  {
    const Attribute object {ObjectRef {100}};
    EXPECT_EQ(object.get_type(), AttributeType::kObject);
    EXPECT_EQ(object.as_object(), ObjectRef {100});
  }
}

// tactile::Attribute::Attribute [const char*]
TEST(Attribute, ImplicitCStringConstructor)
{
  const Attribute attribute {"foobar"};
  EXPECT_FALSE(attribute.has_default_value());
  EXPECT_EQ(attribute.get_type(), AttributeType::kStr);
  EXPECT_EQ(attribute.as_string(), "foobar");
  EXPECT_EQ(attribute, Attribute {"foobar"});
}

// tactile::Attribute::reset
// tactile::Attribute::has_default_value
TEST(Attribute, Reset)
{
  Attribute attribute {};

  attribute.reset(AttributeType::kStr);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {std::string {}});

  attribute.reset(AttributeType::kInt);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {std::int32_t {}});

  attribute.reset(AttributeType::kInt2);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {Int2 {}});

  attribute.reset(AttributeType::kInt3);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {Int3 {}});

  attribute.reset(AttributeType::kInt4);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {Int4 {}});

  attribute.reset(AttributeType::kFloat);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {float {}});

  attribute.reset(AttributeType::kFloat2);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {Float2 {}});

  attribute.reset(AttributeType::kFloat3);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {Float3 {}});

  attribute.reset(AttributeType::kFloat4);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {Float4 {}});

  attribute.reset(AttributeType::kBool);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {bool {}});

  attribute.reset(AttributeType::kColor);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {UColor {}});

  attribute.reset(AttributeType::kPath);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {std::filesystem::path {}});

  attribute.reset(AttributeType::kObject);
  EXPECT_TRUE(attribute.has_default_value());
  EXPECT_EQ(attribute, Attribute {ObjectRef {}});
}

// tactile::Attribute::set
TEST(Attribute, Set)
{
  Attribute attribute {};
  EXPECT_TRUE(attribute.has_default_value());

  attribute.set("foo");
  EXPECT_EQ(attribute, Attribute {"foo"});
  EXPECT_EQ(attribute.get_type(), AttributeType::kStr);
  EXPECT_FALSE(attribute.has_default_value());

  attribute.set(0);
  EXPECT_EQ(attribute, Attribute {0});
  EXPECT_EQ(attribute.get_type(), AttributeType::kInt);
  EXPECT_TRUE(attribute.has_default_value());

  attribute.set(Float3 {1, 2, 3});
  EXPECT_EQ(attribute, Attribute {Float3(1, 2, 3)});
  EXPECT_EQ(attribute.get_type(), AttributeType::kFloat3);
  EXPECT_FALSE(attribute.has_default_value());
}

// tactile::Attribute::as_string
TEST(Attribute, AsString)
{
  EXPECT_EQ(Attribute {AttributeType::kStr}.as_string(), "");
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kInt}.as_string());
}

// tactile::Attribute::as_int
TEST(Attribute, AsInt)
{
  EXPECT_EQ(Attribute {AttributeType::kInt}.as_int(), 0);
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kColor}.as_int());
}

// tactile::Attribute::as_int2
TEST(Attribute, AsInt2)
{
  EXPECT_EQ(Attribute {AttributeType::kInt2}.as_int2(), Int2 {});
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kPath}.as_int2());
}

// tactile::Attribute::as_int3
TEST(Attribute, AsInt3)
{
  EXPECT_EQ(Attribute {AttributeType::kInt3}.as_int3(), Int3 {});
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kInt}.as_int3());
}

// tactile::Attribute::as_int4
TEST(Attribute, AsInt4)
{
  EXPECT_EQ(Attribute {AttributeType::kInt4}.as_int4(), Int4 {});
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kFloat4}.as_int4());
}

// tactile::Attribute::as_float
TEST(Attribute, AsFloat)
{
  EXPECT_EQ(Attribute {AttributeType::kFloat}.as_float(), 0.0f);
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kStr}.as_float());
}

// tactile::Attribute::as_float2
TEST(Attribute, AsFloat2)
{
  EXPECT_EQ(Attribute {AttributeType::kFloat2}.as_float2(), Float2 {});
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kBool}.as_float2());
}

// tactile::Attribute::as_float3
TEST(Attribute, AsFloat3)
{
  EXPECT_EQ(Attribute {AttributeType::kFloat3}.as_float3(), Float3 {});
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kObject}.as_float3());
}

// tactile::Attribute::as_float4
TEST(Attribute, AsFloat4)
{
  EXPECT_EQ(Attribute {AttributeType::kFloat4}.as_float4(), Float4 {});
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kFloat3}.as_float4());
}

// tactile::Attribute::as_bool
TEST(Attribute, AsBool)
{
  EXPECT_EQ(Attribute {AttributeType::kBool}.as_bool(), false);
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kInt}.as_bool());
}

// tactile::Attribute::as_color
TEST(Attribute, AsColor)
{
  EXPECT_EQ(Attribute {AttributeType::kColor}.as_color(), UColor {});
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kStr}.as_color());
}

// tactile::Attribute::as_path
TEST(Attribute, AsPath)
{
  EXPECT_EQ(Attribute {AttributeType::kPath}.as_path(), std::filesystem::path {});
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kStr}.as_path());
}

// tactile::Attribute::as_object
TEST(Attribute, AsObject)
{
  EXPECT_EQ(Attribute {AttributeType::kObject}.as_object(), ObjectRef {});
  EXPECT_ANY_THROW((void) Attribute {AttributeType::kInt}.as_object());
}

// tactile::Attribute::is_vector
TEST(Attribute, IsVector)
{
  EXPECT_TRUE(Attribute {AttributeType::kInt2}.is_vector());
  EXPECT_TRUE(Attribute {AttributeType::kInt3}.is_vector());
  EXPECT_TRUE(Attribute {AttributeType::kInt4}.is_vector());
  EXPECT_TRUE(Attribute {AttributeType::kFloat2}.is_vector());
  EXPECT_TRUE(Attribute {AttributeType::kFloat3}.is_vector());
  EXPECT_TRUE(Attribute {AttributeType::kFloat4}.is_vector());

  EXPECT_FALSE(Attribute {AttributeType::kStr}.is_vector());
  EXPECT_FALSE(Attribute {AttributeType::kInt}.is_vector());
  EXPECT_FALSE(Attribute {AttributeType::kFloat}.is_vector());
  EXPECT_FALSE(Attribute {AttributeType::kBool}.is_vector());
  EXPECT_FALSE(Attribute {AttributeType::kColor}.is_vector());
  EXPECT_FALSE(Attribute {AttributeType::kPath}.is_vector());
  EXPECT_FALSE(Attribute {AttributeType::kObject}.is_vector());
}

}  // namespace
}  // namespace tactile
