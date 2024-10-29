// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/runtime/window_impl.hpp"

#include <SDL2/SDL.h>

#include "tactile/core/debug/validation.hpp"
#include "tactile/runtime/logging.hpp"

namespace tactile::runtime {

void WindowHandleDeleter::operator()(SDL_Window* window) noexcept
{
  SDL_DestroyWindow(window);
}

Window::Window(SDL_Window* window)
  : m_window {core::require_not_null(window, "null window")}
{}

auto Window::create(const std::uint32_t extra_flags) -> std::expected<Window, ErrorCode>
{
  constexpr std::uint32_t basic_flags =
      SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

  auto* handle = SDL_CreateWindow("Tactile " TACTILE_VERSION_STRING,
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  800,
                                  600,
                                  basic_flags | extra_flags);

  if (!handle) {
    TACTILE_RUNTIME_ERROR("Could not create window: {}", SDL_GetError());
    return std::unexpected {ErrorCode::kBadInit};
  }

  return Window {handle};
}

void Window::show()
{
  SDL_ShowWindow(get_handle());
}

void Window::hide()
{
  SDL_HideWindow(get_handle());
}

void Window::maximize()
{
  SDL_MaximizeWindow(get_handle());
}

auto Window::get_handle() -> SDL_Window*
{
  return m_window.get();
}

}  // namespace tactile::runtime
