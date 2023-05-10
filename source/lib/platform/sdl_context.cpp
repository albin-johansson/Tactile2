/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2023 Albin Johansson
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

#include "sdl_context.hpp"

#include <utility>  // move

#include <GL/glew.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "common/debug/panic.hpp"
#include "common/numeric.hpp"
#include "common/predef.hpp"

namespace tactile {
namespace {

inline constexpr uint32 kBaseWindowFlags =
    cen::window::hidden | cen::window::resizable | cen::window::allow_high_dpi;

void _init_sdl_opengl_attributes()
{
  if constexpr (kOnMacos) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  }

  // OpenGL 4.1.0 Core
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
}

}  // namespace

SDLContext::SDLContext(const BackendAPI api)
{
  SDL_SetHint(SDL_HINT_APP_NAME, "Tactile");

  // Enable multi-gesture events from touchpads
  SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");

  if (api == BackendAPI::OpenGL) {
    // Ensure nearest pixel sampling
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    _init_sdl_opengl_attributes();
  }

  auto window_flags = kBaseWindowFlags;
  if (api == BackendAPI::OpenGL) {
    window_flags |= cen::window::opengl;
  }

  auto& window = mWindow.emplace("Tactile", cen::window::default_size(), window_flags);

  if (api == BackendAPI::OpenGL) {
    auto& gl = mGL.emplace(window);
    gl.make_current(window);

    cen::gl::set_swap_interval(cen::gl_swap_interval::synchronized);

    if (const auto result = glewInit(); result != GLEW_OK) {
      auto message = fmt::format("Failed to initialize GLEW: {}",
                                 reinterpret_cast<const char*>(glewGetString(result)));
      throw TactileError {std::move(message)};
    }

    spdlog::debug("[OpenGL] Version: {}",
                  reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    spdlog::debug("[OpenGL] Renderer: {}",
                  reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    // int num_binary_formats {};
    // glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &num_binary_formats);
    // spdlog::debug("[OpenGL] Number of binary shader formats: {}", num_binary_formats);
  }
}

auto SDLContext::get_window() -> cen::window&
{
  return mWindow.value();
}

auto SDLContext::get_gl_context() -> cen::gl_context&
{
  return mGL.value();
}

}  // namespace tactile