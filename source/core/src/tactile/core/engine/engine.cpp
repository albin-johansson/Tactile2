// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/engine/engine.hpp"

#include <SDL2/SDL.h>
#include <imgui.h>

#include "tactile/base/engine/engine_app.hpp"
#include "tactile/base/render/renderer.hpp"
#include "tactile/core/debug/validation.hpp"
#include "tactile/core/log/logger.hpp"

namespace tactile {

Engine::Engine(IEngineApp* app, IRenderer* renderer)
  : mApp {require_not_null(app, "null app")},
    mRenderer {require_not_null(renderer, "null renderer")}
{}

void Engine::run()
{
  ImGui::SetAllocatorFunctions(
      [](const std::size_t bytes, void* user_data) -> void* {
        auto* renderer = static_cast<IRenderer*>(user_data);
        return renderer->imgui_malloc(bytes);
      },
      [](void* memory, void* user_data) {
        auto* renderer = static_cast<IRenderer*>(user_data);
        renderer->imgui_free(memory);
      },
      mRenderer);
  ImGui::SetCurrentContext(mRenderer->get_imgui_context());

  mApp->on_startup();

  bool running = true;
  while (running) {
    running = _poll_events();

    _check_framebuffer_scale();

    mApp->on_update();

    if (mRenderer->begin_frame()) {
      mApp->on_render();
      mRenderer->end_frame();
    }
  }

  mApp->on_shutdown();
}

auto Engine::_poll_events() -> bool
{
  auto keep_running = true;

  SDL_Event event {};
  while (SDL_PollEvent(&event)) {
    mRenderer->process_event(event);

    if (event.type == SDL_QUIT) {
      keep_running = false;
      break;
    }
  }

  return keep_running;
}

void Engine::_check_framebuffer_scale()
{
  const auto& io = ImGui::GetIO();
  const auto current_framebuffer_scale = io.DisplayFramebufferScale.x;

  if (mFramebufferScale != current_framebuffer_scale) {
    mFramebufferScale = current_framebuffer_scale;

    TACTILE_LOG_DEBUG("Framebuffer scale changed to {}", mFramebufferScale);
    mApp->on_framebuffer_scale_changed(mFramebufferScale);
  }
}

}  // namespace tactile
