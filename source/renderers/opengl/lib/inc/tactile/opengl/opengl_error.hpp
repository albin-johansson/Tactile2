// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/debug/error_code.hpp"
#include "tactile/opengl/api.hpp"

namespace tactile::gl {

/**
 * Translates an OpenGL error code, e.g. \c GL_INVALID_ENUM, to a Tactile error code.
 *
 * \param error A native OpenGL error code identifier.
 *
 * \return
 * An error code.
 */
[[nodiscard]]
TACTILE_OPENGL_API auto map_opengl_error_code(unsigned error) noexcept -> ErrorCode;

}  // namespace tactile::gl
