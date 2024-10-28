// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/opengl/opengl_error.hpp"

#include <glad/glad.h>

namespace tactile::gl {

auto map_opengl_error_code(const unsigned error) noexcept -> ErrorCode
{
  switch (error) {
    case GL_INVALID_ENUM:                  [[fallthrough]];
    case GL_INVALID_VALUE:                 return ErrorCode::kBadParam;
    case GL_INVALID_OPERATION:             [[fallthrough]];
    case GL_INVALID_FRAMEBUFFER_OPERATION: return ErrorCode::kBadOperation;
    case GL_STACK_OVERFLOW:                return ErrorCode::kStackOverflow;
    case GL_STACK_UNDERFLOW:               return ErrorCode::kStackUnderflow;
    case GL_OUT_OF_MEMORY:                 return ErrorCode::kOutOfMemory;
  }

  return ErrorCode::kUnknown;
}

}  // namespace tactile::gl
