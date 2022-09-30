/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
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

#include <span>  // span

#include "core/type/string.hpp"

namespace tactile {

/// Zeroes the values of a buffer.
void zero_buffer(std::span<char> buffer);

/// Copies a string into a character buffer.
///
/// This function will zero the entire buffer before writing the string into it.
///
/// Note, the string can safely be longer than what the buffer can hold, the buffer will
/// be filled with as many characters as possible.
///
/// \param buffer the buffer that will be modified.
/// \param str the string that will be copied into the buffer.
void copy_string_into_buffer(std::span<char> buffer, StringView str);

/// Creates a string from a character buffer.
///
/// The function assumes that the "valid" part of the buffer is terminated with a null
/// character ('\0').
///
/// \param buffer the character buffer that contains the string data.
///
/// \return a string created from the buffer.
[[nodiscard]] auto create_string_from_buffer(std::span<const char> buffer) -> String;

/// Creates a string view from a character buffer.
///
/// The function assumes that the "valid" part of the buffer is terminated with a null
/// character ('\0').
///
/// Beware of the lifetime of the pointed-to buffer! The buffer must not be destroyed
/// before the returned string view. This function is useful to obtain a temporary
/// string-like object to perform queries on, use the string overload if you need to store
/// the returned string.
///
/// \param buffer the character buffer that contains the string data.
///
/// \return a string created from the buffer.
[[nodiscard]] auto create_string_view_from_buffer(std::span<const char> buffer)
    -> StringView;

}  // namespace tactile
