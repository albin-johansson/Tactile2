// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/base/numeric/vec.hpp"

#include <format>       // format
#include <sstream>      // stringstream
#include <type_traits>  // is_nothrow_{}_v

#include <gtest/gtest.h>

namespace tactile {
namespace {

// tactile::Vec
TEST(Vec, SpecialMembers)
{
  EXPECT_TRUE(std::is_nothrow_default_constructible_v<Int2>);
  EXPECT_TRUE(std::is_nothrow_default_constructible_v<Int3>);
  EXPECT_TRUE(std::is_nothrow_default_constructible_v<Int4>);
  EXPECT_TRUE(std::is_nothrow_default_constructible_v<Float2>);
  EXPECT_TRUE(std::is_nothrow_default_constructible_v<Float3>);
  EXPECT_TRUE(std::is_nothrow_default_constructible_v<Float4>);

  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<Int2>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<Int3>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<Int4>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<Float2>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<Float3>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<Float4>);

  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<Int2>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<Int3>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<Int4>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<Float2>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<Float3>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<Float4>);

  EXPECT_TRUE(std::is_nothrow_move_constructible_v<Int2>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<Int3>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<Int4>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<Float2>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<Float3>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<Float4>);

  EXPECT_TRUE(std::is_nothrow_move_assignable_v<Int2>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<Int3>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<Int4>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<Float2>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<Float3>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<Float4>);

  EXPECT_TRUE(std::is_nothrow_destructible_v<Int2>);
  EXPECT_TRUE(std::is_nothrow_destructible_v<Int3>);
  EXPECT_TRUE(std::is_nothrow_destructible_v<Int4>);
  EXPECT_TRUE(std::is_nothrow_destructible_v<Float2>);
  EXPECT_TRUE(std::is_nothrow_destructible_v<Float3>);
  EXPECT_TRUE(std::is_nothrow_destructible_v<Float4>);
}

// tactile::Vec::Vec
TEST(Vec, DefaultConstructor)
{
  const Int2 ivec;
  EXPECT_EQ(ivec.x(), 0);
  EXPECT_EQ(ivec.y(), 0);

  const Float4 fvec;
  EXPECT_EQ(fvec.x(), 0.0f);
  EXPECT_EQ(fvec.y(), 0.0f);
  EXPECT_EQ(fvec.z(), 0.0f);
  EXPECT_EQ(fvec.w(), 0.0f);
}

// tactile::Vec::Vec
TEST(Vec, ValueConstructor2D)
{
  const Int2 ivec {1, 2};
  const Float2 fvec {1.0f, 2.0f};

  EXPECT_EQ(ivec.x(), 1);
  EXPECT_EQ(ivec.y(), 2);

  EXPECT_EQ(fvec.x(), 1.0f);
  EXPECT_EQ(fvec.y(), 2.0f);
}

// tactile::Vec::Vec
TEST(Vec, ValueConstructor3D)
{
  const Int3 ivec {1, 2, 3};
  const Float3 fvec {1.0f, 2.0f, 3.0f};

  EXPECT_EQ(ivec.x(), 1);
  EXPECT_EQ(ivec.y(), 2);
  EXPECT_EQ(ivec.z(), 3);

  EXPECT_EQ(fvec.x(), 1.0f);
  EXPECT_EQ(fvec.y(), 2.0f);
  EXPECT_EQ(fvec.z(), 3.0f);
}

// tactile::Vec::Vec
TEST(Vec, ValueConstructor4D)
{
  const Int4 ivec {1, 2, 3, 4};
  const Float4 fvec {1.0f, 2.0f, 3.0f, 4.0f};

  EXPECT_EQ(ivec.x(), 1);
  EXPECT_EQ(ivec.y(), 2);
  EXPECT_EQ(ivec.z(), 3);
  EXPECT_EQ(ivec.w(), 4);

  EXPECT_EQ(fvec.x(), 1.0f);
  EXPECT_EQ(fvec.y(), 2.0f);
  EXPECT_EQ(fvec.z(), 3.0f);
  EXPECT_EQ(fvec.w(), 4.0f);
}

// tactile::Vec::set_x
// tactile::Vec::x
TEST(Vec, SetX)
{
  Float2 vec {};
  vec.set_x(1.5f);

  EXPECT_EQ(vec.x(), 1.5f);
  EXPECT_EQ(vec.y(), 0.0f);
}

// tactile::Vec::set_y
// tactile::Vec::y
TEST(Vec, SetY)
{
  Int2 vec {};
  vec.set_y(42);

  EXPECT_EQ(vec.x(), 0);
  EXPECT_EQ(vec.y(), 42);
}

// tactile::Vec::set_z
// tactile::Vec::z
TEST(Vec, SetZ)
{
  Int3 vec {};
  vec.set_z(123);

  EXPECT_EQ(vec.x(), 0);
  EXPECT_EQ(vec.y(), 0);
  EXPECT_EQ(vec.z(), 123);
}

// tactile::Vec::set_w
// tactile::Vec::w
TEST(Vec, SetW)
{
  Float4 vec {};
  vec.set_w(25.8f);

  EXPECT_EQ(vec.x(), 0.0f);
  EXPECT_EQ(vec.y(), 0.0f);
  EXPECT_EQ(vec.z(), 0.0f);
  EXPECT_EQ(vec.w(), 25.8f);
}

// tactile::Vec::at
TEST(Vec, At)
{
  Int4 vec {};
  vec.set_x(10);
  vec.set_y(20);
  vec.set_z(30);
  vec.set_w(40);

  EXPECT_EQ(vec.at(0), 10);
  EXPECT_EQ(vec.at(1), 20);
  EXPECT_EQ(vec.at(2), 30);
  EXPECT_EQ(vec.at(3), 40);

  EXPECT_ANY_THROW((void) vec.at(4));
  EXPECT_ANY_THROW((void) vec.at(vec.size()));
}

// tactile::Vec::operator[]
TEST(Vec, SubscriptOperator)
{
  Float4 vec {};
  vec[0] = 1.0f;
  vec[1] = 2.0f;
  vec[2] = 3.0f;
  vec[3] = 4.0f;

  EXPECT_EQ(vec[0], 1.0f);
  EXPECT_EQ(vec[1], 2.0f);
  EXPECT_EQ(vec[2], 3.0f);
  EXPECT_EQ(vec[3], 4.0f);

  const auto& const_vec = vec;
  EXPECT_EQ(const_vec[0], 1.0f);
  EXPECT_EQ(const_vec[1], 2.0f);
  EXPECT_EQ(const_vec[2], 3.0f);
  EXPECT_EQ(const_vec[3], 4.0f);
}

// tactile::Vec::data
TEST(Vec, Data)
{
  Int3 vec {};
  vec[0] = 12;
  vec[1] = 34;
  vec[2] = 56;

  const auto& const_vec = vec;

  auto* data = vec.data();
  ASSERT_NE(data, nullptr);
  EXPECT_EQ(data[0], 12);
  EXPECT_EQ(data[1], 34);
  EXPECT_EQ(data[2], 56);

  const auto* const_data = const_vec.data();
  ASSERT_NE(const_data, nullptr);
  EXPECT_EQ(const_data[0], 12);
  EXPECT_EQ(const_data[1], 34);
  EXPECT_EQ(const_data[2], 56);
}

// tactile::Vec::size
TEST(Vec, Size)
{
  EXPECT_EQ(Int2 {}.size(), 2);
  EXPECT_EQ(Int3 {}.size(), 3);
  EXPECT_EQ(Int4 {}.size(), 4);

  EXPECT_EQ(Float2 {}.size(), 2);
  EXPECT_EQ(Float3 {}.size(), 3);
  EXPECT_EQ(Float4 {}.size(), 4);
}

// tactile::Vec::operator-
TEST(Vec, UnaryMinus)
{
  const Int3 vec {-1, 2, 3};
  const auto negated_vec = -vec;

  EXPECT_EQ(negated_vec.x(), 1);
  EXPECT_EQ(negated_vec.y(), -2);
  EXPECT_EQ(negated_vec.z(), -3);
}

// tactile::operator== [Vec]
TEST(Vec, EqualityOperator)
{
  const Int3 a {-83, 934, -193};
  const Int3 b {64, 8, 152};

  EXPECT_EQ(a, a);
  EXPECT_EQ(b, b);

  EXPECT_FALSE(a == b);
  EXPECT_FALSE(b == a);
}

// tactile::operator!= [Vec]
TEST(Vec, InequalityOperator)
{
  const Float2 a {1.0f, 2.0f};
  const Float2 b {3.0f, 4.0f};

  EXPECT_NE(a, b);
  EXPECT_NE(b, a);

  EXPECT_FALSE(a != a);
  EXPECT_FALSE(b != b);
}

// tactile::operator<(const Vec&, const Vec&)
TEST(Vec, LessThanOperator)
{
  const Float2 a {1.0f, 0.0f};
  const Float2 b {2.0f, 0.0f};
  const Float2 c {0.0f, 1.0f};
  const Float2 d {0.0f, 2.0f};

  EXPECT_LT(a, b);

  EXPECT_LT(c, a);
  EXPECT_LT(c, b);
  EXPECT_LT(c, d);

  EXPECT_LT(d, a);
  EXPECT_LT(d, b);
}

// tactile::operator<= [Vec]
TEST(Vec, LessThanEqOperator)
{
  const Int2 a {1, 0};
  const Int2 b {2, 0};
  const Int2 c {0, 1};
  const Int2 d {0, 2};

  EXPECT_LE(a, a);
  EXPECT_LE(b, b);
  EXPECT_LE(c, c);
  EXPECT_LE(d, d);

  EXPECT_LE(a, b);

  EXPECT_LE(c, a);
  EXPECT_LE(c, b);
  EXPECT_LE(c, d);

  EXPECT_LE(d, a);
  EXPECT_LE(d, b);
}

// tactile::operator> [Vec]
TEST(Vec, GreaterThanOperator)
{
  const Float2 a {1.0f, 0.0f};
  const Float2 b {2.0f, 0.0f};
  const Float2 c {0.0f, 1.0f};
  const Float2 d {0.0f, 2.0f};

  EXPECT_GT(a, c);
  EXPECT_GT(a, d);

  EXPECT_GT(b, a);
  EXPECT_GT(b, c);
  EXPECT_GT(b, d);

  EXPECT_GT(d, c);
}

// tactile::operator>= [Vec]
TEST(Vec, GreaterThanEqOperator)
{
  const Int2 a {1, 0};
  const Int2 b {2, 0};
  const Int2 c {0, 1};
  const Int2 d {0, 2};

  EXPECT_GE(a, a);
  EXPECT_GE(b, b);
  EXPECT_GE(c, c);
  EXPECT_GE(d, d);

  EXPECT_GE(a, c);
  EXPECT_GE(a, d);

  EXPECT_GE(b, a);
  EXPECT_GE(b, c);
  EXPECT_GE(b, d);

  EXPECT_GE(d, c);
}

// tactile::operator+ [Vec]
TEST(Vec, AdditionOperator)
{
  const Int4 a {-5, 10, 73, -42};
  const Int4 b {3, 49, 31, -18};

  const auto ab = a + b;
  const auto ba = b + a;

  EXPECT_EQ(ab.x(), a.x() + b.x());
  EXPECT_EQ(ab.y(), a.y() + b.y());
  EXPECT_EQ(ab.z(), a.z() + b.z());
  EXPECT_EQ(ab.w(), a.w() + b.w());

  EXPECT_EQ(ab, ba);
}

// tactile::operator- [Vec]
TEST(Vec, SubtractionOperator)
{
  const Float3 a {54.3f, -20.1f, 89.6f};
  const Float3 b {-27.6f, 54.5f, 12.2f};

  const auto ab = a - b;
  const auto ba = b - a;

  EXPECT_EQ(ab.x(), a.x() - b.x());
  EXPECT_EQ(ab.y(), a.y() - b.y());
  EXPECT_EQ(ab.z(), a.z() - b.z());

  EXPECT_EQ(ba.x(), b.x() - a.x());
  EXPECT_EQ(ba.y(), b.y() - a.y());
  EXPECT_EQ(ba.z(), b.z() - a.z());

  EXPECT_NE(ab, ba);
}

// tactile::operator* [Vec]
TEST(Vec, MultiplicationOperator)
{
  const Float2 a {1.0f, 43.0f};
  const Float2 b {-7.3f, 7.8f};

  const auto ab = a * b;
  const auto ba = b * a;

  EXPECT_EQ(ab.x(), a.x() * b.x());
  EXPECT_EQ(ab.y(), a.y() * b.y());

  EXPECT_EQ(ab, ba);
}

// tactile::operator* [Vec]
TEST(Vec, MultiplicationWithScalarOperator)
{
  const Float4 vec {1.0f, 2.0f, 3.0f, 4.0f};
  const auto scalar = 10.0f;

  const auto scaled_vec1 = vec * scalar;
  const auto scaled_vec2 = scalar * vec;

  EXPECT_EQ(scaled_vec1.x(), vec.x() * scalar);
  EXPECT_EQ(scaled_vec1.y(), vec.y() * scalar);
  EXPECT_EQ(scaled_vec1.z(), vec.z() * scalar);
  EXPECT_EQ(scaled_vec1.w(), vec.w() * scalar);

  EXPECT_EQ(scaled_vec1, scaled_vec2);
}

// tactile::operator*=
TEST(Vec, ScalarMultiplicationAssignment)
{
  Float3 vec {1.0f, -2.0f, 3.0f};

  vec *= 2.0f;
  EXPECT_EQ(vec.x(), 2.0f);
  EXPECT_EQ(vec.y(), -4.0f);
  EXPECT_EQ(vec.z(), 6.0f);
}

// tactile::operator/ [Vec]
TEST(Vec, DivisionOperator)
{
  const Float4 a {1.0f, 2.0f, 3.0f, 4.0f};
  const Float4 b {0.1f, 0.2f, 0.3f, 0.4f};

  const auto ab = a / b;
  const auto ba = b / a;

  EXPECT_EQ(ab.x(), a.x() / b.x());
  EXPECT_EQ(ab.y(), a.y() / b.y());
  EXPECT_EQ(ab.z(), a.z() / b.z());
  EXPECT_EQ(ab.w(), a.w() / b.w());

  EXPECT_EQ(ba.x(), b.x() / a.x());
  EXPECT_EQ(ba.y(), b.y() / a.y());
  EXPECT_EQ(ba.z(), b.z() / a.z());
  EXPECT_EQ(ba.w(), b.w() / a.w());

  EXPECT_NE(ab, ba);
}

// tactile::operator/=
TEST(Vec, ScalarDivisionAssignment)
{
  Float4 vec {1.0f, 2.0f, 3.0f, 4.0f};

  vec /= 2.0f;
  EXPECT_FLOAT_EQ(vec.x(), 0.5f);
  EXPECT_FLOAT_EQ(vec.y(), 1.0f);
  EXPECT_FLOAT_EQ(vec.z(), 1.5f);
  EXPECT_FLOAT_EQ(vec.w(), 2.0f);
}

// tactile::apply [Vec]
TEST(Vec, Apply)
{
  const Float3 a {1.0f, 5.0f, 10.0f};
  const Float3 b = apply(a, [](const float f) { return f * 2.0f; });

  EXPECT_EQ(b.x(), 2.0f);
  EXPECT_EQ(b.y(), 10.0f);
  EXPECT_EQ(b.z(), 20.0f);
}

// tactile::apply2 [Vec]
TEST(Vec, Apply2)
{
  const Int4 a {2, 24, 8, 12};
  const Int4 b {4, 24, 5, 16};
  const Int4 c = apply2(a, b, [](const int i1, const int i2) { return i1 % i2; });

  EXPECT_EQ(c.x(), 2);
  EXPECT_EQ(c.y(), 0);
  EXPECT_EQ(c.z(), 3);
  EXPECT_EQ(c.w(), 12);
}

// tactile::vec_cast
TEST(Vec, VecCast)
{
  const Int4 ivec {10, 20, 30, 40};
  const auto fvec = vec_cast<Float4>(ivec);

  EXPECT_EQ(fvec.x(), static_cast<float>(ivec.x()));
  EXPECT_EQ(fvec.y(), static_cast<float>(ivec.y()));
  EXPECT_EQ(fvec.z(), static_cast<float>(ivec.z()));
  EXPECT_EQ(fvec.w(), static_cast<float>(ivec.w()));
}

}  // namespace
}  // namespace tactile
