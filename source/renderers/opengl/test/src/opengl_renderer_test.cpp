// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/opengl/opengl_renderer.hpp"

#include <gtest/gtest.h>

#include "tactile/opengl/opengl_renderer_plugin.hpp"
#include "tactile/runtime/command_line_options.hpp"
#include "tactile/runtime/runtime_impl.hpp"

namespace tactile::gl {
namespace {

class OpenGLRendererTest : public testing::Test
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
};

// tactile::gl::OpenGLRenderer::load_texture
TEST_F(OpenGLRendererTest, LoadTexture)
{
  auto* renderer = dynamic_cast<OpenGLRenderer*>(m_runtime.get_renderer());
  ASSERT_NE(renderer, nullptr);

  EXPECT_EQ(renderer->load_texture("assets/images/dummy.png"), TextureID {1});
  EXPECT_FALSE(renderer->load_texture("a/b/c.png").has_value());
}

}  // namespace
}  // namespace tactile::gl
