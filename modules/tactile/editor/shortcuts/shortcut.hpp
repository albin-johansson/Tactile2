#pragma once

#include <SDL.h>
#include <centurion.hpp>  // scan_code, key_mod, keyboard_event
#include <entt/entt.hpp>  // dispatcher
#include <tactile_def.hpp>

namespace Tactile {

class Model;
class WidgetManager;

class AShortcut {
 public:
  explicit AShortcut(const cen::ScanCode key, const uint16 modifiers = KMOD_NONE)
      : mKey{key}
      , mModifiers{modifiers}
  {}

  virtual ~AShortcut() noexcept = default;

  void Poll(const Model& model,
            const WidgetManager& widgets,
            const SDL_KeyboardEvent& event,
            entt::dispatcher& dispatcher);

  virtual void Activate(entt::dispatcher& dispatcher) = 0;

  [[nodiscard]] virtual auto IsEnabled(
      [[maybe_unused]] const Model& model,
      [[maybe_unused]] const WidgetManager& widgets) const -> bool
  {
    return true;
  }

 private:
  cen::ScanCode mKey;
  uint16 mModifiers{KMOD_NONE};
};

}  // namespace Tactile
