// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/opengl/opengl_texture.hpp"

#include <gtest/gtest.h>

#include "tactile/opengl/opengl_renderer_plugin.hpp"
#include "tactile/runtime/command_line_options.hpp"
#include "tactile/runtime/runtime_impl.hpp"

namespace tactile::gl {
namespace {

class OpenGLTextureTest : public testing::Test
{
 protected:
  void SetUp() override
  {
    m_plugin.load(&m_runtime);
  }

  void TearDown() override
  {
    m_plugin.unload();
  }

  runtime::RuntimeImpl m_runtime {runtime::get_default_command_line_options()};
  OpenGLRendererPlugin m_plugin {};
  RendererOptions m_options {};
};

// tactile::gl::OpenGLTexture::load
TEST_F(OpenGLTextureTest, Load)
{
  const auto texture = OpenGLTexture::load("assets/images/dummy.png", m_options);
  ASSERT_TRUE(texture.has_value());
  EXPECT_NE(texture->get_handle(), nullptr);
  EXPECT_EQ(texture->get_path(), "assets/images/dummy.png");
  EXPECT_EQ(texture->get_size().width, 96);
  EXPECT_EQ(texture->get_size().height, 64);
}

}  // namespace
}  // namespace tactile::gl
