// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/opengl/opengl_renderer_plugin.hpp"

#include <new>  // nothrow

#include <SDL2/SDL.h>
#include <imgui.h>

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/opengl/logging.hpp"

namespace tactile {
namespace {

void _set_hints()
{
  if constexpr (kOnMacos) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
}

}  // namespace

void OpenGLRendererPlugin::load(IRuntime* runtime)
{
  m_runtime = runtime;
  gl::set_logger(runtime->get_logger());

  _set_hints();

  m_runtime->init_window(SDL_WINDOW_OPENGL);
  auto* window = m_runtime->get_window();

  if (!window) {
    TACTILE_OPENGL_ERROR("Could not initialize OpenGL window");
    return;
  }

  ImGuiMemAllocFunc imgui_alloc_fn {};
  ImGuiMemFreeFunc imgui_free_fn {};
  void* imgui_user_data {};
  m_runtime->get_imgui_allocator_functions(&imgui_alloc_fn, &imgui_free_fn, &imgui_user_data);
  ImGui::SetAllocatorFunctions(imgui_alloc_fn, imgui_free_fn, imgui_user_data);

  if (auto renderer = OpenGLRenderer::make(m_runtime->get_renderer_options(), window)) {
    m_renderer = std::move(*renderer);
    m_runtime->set_renderer(&m_renderer.value());
  }
}

void OpenGLRendererPlugin::unload()
{
  m_runtime->set_renderer(nullptr);
  m_renderer.reset();

  gl::set_logger(nullptr);
  m_runtime = nullptr;
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) OpenGLRendererPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile
