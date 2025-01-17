// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/prelude.hpp"

struct SDL_Window;

namespace tactile {

/**
 * Provides the high level window API.
 */
class IWindow
{
 public:
  TACTILE_INTERFACE_CLASS(IWindow);

  /**
   * Makes the window visible.
   */
  virtual void show() = 0;

  /**
   * Makes the window invisible.
   */
  virtual void hide() = 0;

  /**
   * Maximizes the window size within the available region.
   */
  virtual void maximize() = 0;

  /**
   * Returns the associated window handle.
   *
   * \return
   * A window handle.
   */
  [[nodiscard]]
  virtual auto get_handle() -> SDL_Window* = 0;
};

}  // namespace tactile
