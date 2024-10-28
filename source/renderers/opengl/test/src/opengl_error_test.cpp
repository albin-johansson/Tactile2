// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/opengl/opengl_error.hpp"

#include <glad/glad.h>
#include <gtest/gtest.h>

namespace tactile::gl {
namespace {

// tactile::gl::map_opengl_error_code
TEST(OpenGLError, MapOpenGLErrorCode)
{
  EXPECT_EQ(map_opengl_error_code(GL_INVALID_ENUM), ErrorCode::kBadParam);
  EXPECT_EQ(map_opengl_error_code(GL_INVALID_VALUE), ErrorCode::kBadParam);
  EXPECT_EQ(map_opengl_error_code(GL_INVALID_OPERATION), ErrorCode::kBadOperation);
  EXPECT_EQ(map_opengl_error_code(GL_STACK_OVERFLOW), ErrorCode::kStackOverflow);
  EXPECT_EQ(map_opengl_error_code(GL_STACK_UNDERFLOW), ErrorCode::kStackUnderflow);
  EXPECT_EQ(map_opengl_error_code(GL_OUT_OF_MEMORY), ErrorCode::kOutOfMemory);
  EXPECT_EQ(map_opengl_error_code(GL_INVALID_FRAMEBUFFER_OPERATION), ErrorCode::kBadOperation);
}

}  // namespace
}  // namespace tactile::gl
