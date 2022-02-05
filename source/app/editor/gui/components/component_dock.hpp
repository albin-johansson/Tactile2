#pragma once

#include "editor/gui/dock_widget.hpp"

namespace tactile {

class component_dock final : public dock_widget
{
 public:
  component_dock();

 protected:
  void on_update(const entt::registry& registry, entt::dispatcher& dispatcher) override;

  void set_visible(bool visible) override;

  [[nodiscard]] auto is_visible() const -> bool override;
};

}  // namespace tactile
