/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
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

#pragma once

#include <entt/fwd.hpp>

#include "core/fwd.hpp"
#include "core/tools/tool_type.hpp"

namespace tactile {

class ATool
{
 public:
  virtual ~ATool() noexcept = default;

  virtual void draw_gizmos(const entt::registry& registry,
                           IRenderer& renderer,
                           const MouseInfo& mouse) const;

  virtual void on_enabled(entt::registry& registry, entt::dispatcher& dispatcher);

  virtual void on_disabled(entt::registry& registry, entt::dispatcher& dispatcher);

  virtual void on_entered(entt::registry& registry, entt::dispatcher& dispatcher);

  virtual void on_exited(entt::registry& registry, entt::dispatcher& dispatcher);

  virtual void on_pressed(entt::registry& registry,
                          entt::dispatcher& dispatcher,
                          const MouseInfo& mouse);

  virtual void on_dragged(entt::registry& registry,
                          entt::dispatcher& dispatcher,
                          const MouseInfo& mouse);

  virtual void on_released(entt::registry& registry,
                           entt::dispatcher& dispatcher,
                           const MouseInfo& mouse);

  [[nodiscard]] virtual auto is_available(const entt::registry& registry) const
      -> bool = 0;

  [[nodiscard]] virtual auto get_type() const -> ToolType = 0;
};

}  // namespace tactile