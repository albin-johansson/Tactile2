// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <stdexcept>  // invalid_argument

namespace tactile {

/**
 * Throws an exception if the provided pointer is null, returns it if not.
 *
 * \note
 * This function is particularly useful for use in constructor member
 * initializer lists, as in the following example.
 *
 * \code{cpp}
 * A::A(B* b, C* c)
 *   : _b{require_not_null(b)},
 *     _c{require_not_null(c)}
 * {}
 * \endcode
 *
 * \tparam Pointer A pointer-like type.
 *
 * \param ptr           The pointer to validate.
 * \param error_message An error message used if the pointer is null.
 *
 * \return
 * The provided pointer.
 */
template <typename Pointer>
[[nodiscard]] auto require_not_null(Pointer ptr, const char* error_message = "null pointer")
    -> Pointer
{
  if (ptr == nullptr) [[unlikely]] {
    throw std::invalid_argument {error_message};
  }

  return ptr;
}

}  // namespace tactile
