// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/runtime/sdl_context.hpp"

#include <stdexcept>  // runtime_error

#include <SDL2/SDL.h>

#include "tactile/runtime/logging.hpp"

namespace tactile::runtime {

SDLContext::SDLContext()
{
  TACTILE_RUNTIME_TRACE("Initializing SDL library");

  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0) {
    TACTILE_RUNTIME_ERROR("Could not initialize SDL: {}", SDL_GetError());
    throw std::runtime_error {"Could not initialize SDL"};
  }

  SDL_SetHint(SDL_HINT_APP_NAME, "Tactile");
  SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
}

SDLContext::~SDLContext() noexcept
{
  SDL_Quit();
}

}  // namespace tactile::runtime
