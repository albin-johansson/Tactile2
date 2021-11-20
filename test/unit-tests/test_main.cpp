#include <cstdlib>  // atexit

#include <centurion.hpp>  // library
#include <GL/glew.h>      // glewInit
#include <gtest/gtest.h>

#include "cfg/gl_attributes.hpp"
#include "core/utils/load_texture.hpp"
#include "io/preferences.hpp"

auto main(int argc, char** argv) -> int
{
  std::atexit(Tactile::UnloadTextures);

  const cen::library centurion;

  Tactile::InitOpenGLAttributes();

  auto flags = cen::window::opengl | cen::window::hidden | cen::window::resizable;
  if constexpr (cen::ifdef_apple()) {
    flags |= cen::window::high_dpi;
  }

  cen::window window{"Tactile tests", cen::window::default_size(), flags};

  cen::gl_context context{window};
  context.make_current(window);

  cen::gl::set_swap_interval(cen::gl_swap_interval::synchronized);

  if (glewInit() != GLEW_OK) {
    cen::log::error("Failed to initialize GLEW!");
    return -1;
  }

  Tactile::LoadPreferences();

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}