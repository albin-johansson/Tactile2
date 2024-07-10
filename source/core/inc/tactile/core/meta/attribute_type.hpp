// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <ostream>  // ostream

#include "tactile/base/container/expected.hpp"
#include "tactile/base/container/string.hpp"
#include "tactile/base/int.hpp"
#include "tactile/base/meta/attribute_type.hpp"
#include "tactile/base/prelude.hpp"

namespace tactile {

/**
 * Parses an attribute type from a string.
 *
 * \param name The attribute type name.
 *
 * \return
 * An attribute type if successful; an error code otherwise.
 */
[[nodiscard]]
auto parse_attribute_type(StringView name) -> Result<AttributeType>;

/**
 * Converts an attribute type to a string.
 *
 * \details
 * The returned string is guaranteed to be null-terminated.
 *
 * \param type An attribute type.
 *
 * \return
 * A string.
 */
[[nodiscard]]
auto serialize(AttributeType type) -> StringView;

/**
 * Outputs an attribute type to a stream for debugging purposes.
 *
 * \param stream The output stream to use.
 * \param type   The attribute type to output.
 *
 * \return
 * The provided stream.
 */
auto operator<<(std::ostream& stream, AttributeType type) -> std::ostream&;

}  // namespace tactile
