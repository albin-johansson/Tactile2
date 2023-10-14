// Copyright (C) 2023 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <concepts>  // same_as, convertible_to
#include <ostream>   // ostream
#include <utility>   // move

#include "tactile/core/api.hpp"
#include "tactile/core/container/string.hpp"
#include "tactile/core/container/variant.hpp"
#include "tactile/core/context/attribute_type.hpp"
#include "tactile/core/context/color.hpp"
#include "tactile/core/debug/assert.hpp"
#include "tactile/core/debug/error.hpp"
#include "tactile/core/functional/maybe.hpp"
#include "tactile/core/io/filesystem.hpp"
#include "tactile/core/math/vector.hpp"
#include "tactile/core/prelude.hpp"

namespace tactile {

template <typename T>
concept AttributeValueType = std::same_as<T, bool> ||             //
                             std::convertible_to<T, String> ||    //
                             std::convertible_to<T, int32> ||     //
                             std::convertible_to<T, Int2> ||      //
                             std::convertible_to<T, Int3> ||      //
                             std::convertible_to<T, Int4> ||      //
                             std::convertible_to<T, float32> ||   //
                             std::convertible_to<T, Float2> ||    //
                             std::convertible_to<T, Float3> ||    //
                             std::convertible_to<T, Float4> ||    //
                             std::convertible_to<T, UColor> ||    //
                             std::convertible_to<T, FilePath> ||  //
                             std::convertible_to<T, ObjectRef>;

/**
 * \brief Represents a generic value of one of several possible types.
 */
class Attribute final {
  // These are indices into the value type variant
  inline static constexpr usize kStringTypeIndex = 0;
  inline static constexpr usize kIntTypeIndex = 1;
  inline static constexpr usize kInt2TypeIndex = 2;
  inline static constexpr usize kInt3TypeIndex = 3;
  inline static constexpr usize kInt4TypeIndex = 4;
  inline static constexpr usize kFloatTypeIndex = 5;
  inline static constexpr usize kFloat2TypeIndex = 6;
  inline static constexpr usize kFloat3TypeIndex = 7;
  inline static constexpr usize kFloat4TypeIndex = 8;
  inline static constexpr usize kBoolTypeIndex = 9;
  inline static constexpr usize kColorTypeIndex = 10;
  inline static constexpr usize kPathTypeIndex = 11;
  inline static constexpr usize kObjRefTypeIndex = 12;

 public:
  using string_type = String;
  using int_type = int32;
  using int2_type = Int2;
  using int3_type = Int3;
  using int4_type = Int4;
  using float_type = float32;
  using float2_type = Float2;
  using float3_type = Float3;
  using float4_type = Float4;
  using color_type = UColor;
  using path_type = FilePath;
  using objref_type = ObjectRef;

  // Remember to update the type indices if the type order changes.
  using variant_type = Variant<string_type,
                               int_type,
                               int2_type,
                               int3_type,
                               int4_type,
                               float_type,
                               float2_type,
                               float3_type,
                               float4_type,
                               bool,
                               color_type,
                               path_type,
                               objref_type>;

  /** \brief Creates an empty string attribute. */
  Attribute() = default;

  /**
   * \brief Creates an attribute with the default value of the specified type.
   *
   * \param type the underlying value type.
   */
  explicit Attribute(const AttributeType type)
  {
    reset(type);
  }

  /**
   * \brief Creates an attribute with a specific value.
   *
   * \param value the initial value.
   */
  template <AttributeValueType T>
  Attribute(T value)
  {
    mValue.emplace<T>(std::move(value));
  }

  /**
   * \brief Creates a string attribute.
   *
   * \pre The provided string cannot be null.
   *
   * \param str a C-style string.
   */
  Attribute(const char* str)
  {
    TACTILE_ASSERT(str);
    mValue.emplace<string_type>(str);
  }

  /**
   * \brief Resets the attribute the default value for the specified type.
   *
   * \param type the underlying value type.
   */
  TACTILE_CORE_API void reset(AttributeType type);

  /**
   * \brief Updates the value of the attribute.
   *
   * \param value the new value.
   */
  template <AttributeValueType T>
  void set(T value)
  {
    mValue.emplace<T>(std::move(value));
  }

  /**
   * \brief Updates the value of the attribute.
   *
   * \pre The provided string cannot be null.
   *
   * \param value the new string value.
   */
  void set(const char* str)
  {
    TACTILE_ASSERT(str);
    mValue.emplace<string_type>(str);
  }

  /**
   * \brief Returns the underlying string value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_string() const -> const string_type&
  {
    return _get<string_type>();
  }

  /**
   * \brief Returns the underlying int value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_int() const -> int_type
  {
    return _get<int_type>();
  }

  /**
   * \brief Returns the underlying 2D int vector value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_int2() const -> const int2_type&
  {
    return _get<int2_type>();
  }

  /**
   * \brief Returns the underlying 3D int vector value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_int3() const -> const int3_type&
  {
    return _get<int3_type>();
  }

  /**
   * \brief Returns the underlying 4D int vector value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_int4() const -> const int4_type&
  {
    return _get<int4_type>();
  }

  /**
   * \brief Returns the underlying float value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_float() const -> float_type
  {
    return _get<float_type>();
  }

  /**
   * \brief Returns the underlying 2D float vector value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_float2() const -> const float2_type&
  {
    return _get<float2_type>();
  }

  /**
   * \brief Returns the underlying 3D float vector value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_float3() const -> const float3_type&
  {
    return _get<float3_type>();
  }

  /**
   * \brief Returns the underlying 4D float vector value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_float4() const -> const float4_type&
  {
    return _get<float4_type>();
  }

  /**
   * \brief Returns the underlying boolean value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_bool() const -> bool
  {
    return _get<bool>();
  }

  /**
   * \brief Returns the underlying color value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_color() const -> const color_type&
  {
    return _get<color_type>();
  }

  /**
   * \brief Returns the underlying file path value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_path() const -> const path_type&
  {
    return _get<path_type>();
  }

  /**
   * \brief Returns the underlying object reference value.
   *
   * \return the underlying value.
   *
   * \throws Error if the attribute is of a different type.
   */
  [[nodiscard]]
  auto as_object() const -> objref_type
  {
    return _get<objref_type>();
  }

  /**
   * \brief Indicates whether the attribute is a vector attribute.
   *
   * \return true if the attribute has a vector value; false otherwise.
   */
  [[nodiscard]]
  TACTILE_CORE_API auto is_vector() const -> bool;

  /**
   * \brief Indicates whether the attribute has the default value for its type.
   *
   * \return true if the value is the default one; false otherwise.
   */
  [[nodiscard]]
  TACTILE_CORE_API auto has_default_value() const -> bool;

  /**
   * \brief Returns the type of the current value.
   *
   * \return the current type.
   */
  [[nodiscard]]
  TACTILE_CORE_API auto get_type() const -> AttributeType;

  [[nodiscard]] auto operator==(const Attribute&) const -> bool = default;

 private:
  variant_type mValue;

  template <AttributeValueType T>
  [[nodiscard]]
  auto _get() const -> const T&
  {
    if (const auto* value = get_if<T>(&mValue)) {
      return *value;
    }

    throw Error {"Attribute type mismatch"};
  }
};

TACTILE_CORE_API auto operator<<(std::ostream& stream, const Attribute& attribute)
    -> std::ostream&;

}  // namespace tactile