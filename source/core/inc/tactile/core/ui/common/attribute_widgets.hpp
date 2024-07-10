// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/container/maybe.hpp"
#include "tactile/base/meta/attribute_type.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/core/meta/attribute.hpp"

namespace tactile::ui {

class Language;

/**
 * Pushes an attribute string input to the widget stack.
 *
 * \param id  The widget identifier.
 * \param str The current string value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_string_input(const char* id, const Attribute::string_type& str)
    -> Optional<Attribute::string_type>;

/**
 * Pushes an attribute integer input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current integer value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_int_input(const char* id,
                    Attribute::int_type value) -> Optional<Attribute::int_type>;

/**
 * Pushes an attribute 2D integer input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current 2D integer value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_int2_input(const char* id, Attribute::int2_type value)
    -> Optional<Attribute::int2_type>;

/**
 * Pushes an attribute 3D integer input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current 3D integer value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_int3_input(const char* id, Attribute::int3_type value)
    -> Optional<Attribute::int3_type>;

/**
 * Pushes an attribute 4D integer input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current 4D integer value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_int4_input(const char* id, Attribute::int4_type value)
    -> Optional<Attribute::int4_type>;

/**
 * Pushes an attribute float input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current float value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_float_input(const char* id, Attribute::float_type value)
    -> Optional<Attribute::float_type>;

/**
 * Pushes an attribute 2D float input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current 2D float value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_float2_input(const char* id, Attribute::float2_type value)
    -> Optional<Attribute::float2_type>;

/**
 * Pushes an attribute 3D float input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current 3D float value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_float3_input(const char* id, Attribute::float3_type value)
    -> Optional<Attribute::float3_type>;

/**
 * Pushes an attribute 4D float input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current 4D float value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_float4_input(const char* id, Attribute::float4_type value)
    -> Optional<Attribute::float4_type>;

/**
 * Pushes an attribute boolean input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current boolean value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_bool_input(const char* id, bool value) -> Optional<bool>;

/**
 * Pushes an attribute path input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param value The current path value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_path_input(const char* id, const Attribute::path_type& path)
    -> Optional<Attribute::path_type>;

/**
 * Pushes an attribute color input to the widget stack.
 *
 * \param id    The widget identifier.
 * \param color The current color value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_color_input(const char* id, const Attribute::color_type& color)
    -> Optional<Attribute::color_type>;

/**
 * Pushes an attribute object input to the widget stack.
 *
 * \param id     The widget identifier.
 * \param object The current object value.
 *
 * \return
 * The new value if modified; an empty optional otherwise.
 */
auto push_objref_input(const char* id, const Attribute::objref_type& object)
    -> Optional<Attribute::objref_type>;

/**
 * Pushes a generic attribute input widget to the widget stack.
 *
 * \param id        The widget identifier.
 * \param attribute The current attribute value.
 *
 * \return
 * The new attribute value if modified; an empty optional otherwise.
 */
auto push_attribute_input(const char* id,
                          const Attribute& attribute) -> Optional<Attribute>;

/**
 * Pushes a combo for selecting an attribute type to the widget stack.
 *
 * \param language The current language.
 * \param label    The combo label.
 * \param type     The attribute type to control.
 *
 * \return
 * True if the selected attribute type changed; false otherwise.
 */
auto push_attribute_type_combo(const Language& language,
                               const char* label,
                               AttributeType& type) -> bool;

}  // namespace tactile::ui
