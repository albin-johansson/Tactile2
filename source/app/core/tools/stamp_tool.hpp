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

#include <optional>  // optional

#include "core/common/tile_cache.hpp"
#include "core/tools/tool.hpp"

namespace tactile {

/**
 * \brief A tool used to draw (or "stamp") selected tileset tiles to tile layers.
 *
 * \details The stamp tool, in its default mode, will simply stamp the selected tiles in
 * the active tileset to tile layers. It also has a "random" mode, where a single tile is
 * randomly picked from the tile selection, and subsequently stamped to the tile layer.
 *
 * \note Changes are applied to the target tile layer immediately, and later submitted
 * as a command when the user ends the sequence.
 *
 * \todo Right-click should erase tiles.
 */
class StampTool final : public ATool
{
 public:
  void draw_gizmos(const entt::registry& registry,
                   IRenderer& renderer,
                   const MouseInfo& mouse) const override;

  void on_disabled(entt::registry& registry, entt::dispatcher& dispatcher) override;

  void on_exited(entt::registry& registry, entt::dispatcher& dispatcher) override;

  void on_pressed(entt::registry& registry,
                  entt::dispatcher& dispatcher,
                  const MouseInfo& mouse) override;

  void on_dragged(entt::registry& registry,
                  entt::dispatcher& dispatcher,
                  const MouseInfo& mouse) override;

  void on_released(entt::registry& registry,
                   entt::dispatcher& dispatcher,
                   const MouseInfo& mouse) override;

  [[nodiscard]] auto is_available(const entt::registry& registry) const -> bool override;

  [[nodiscard]] auto get_type() const -> ToolType override;

  void set_random(bool random);

  [[nodiscard]] auto is_random() const -> bool;

 private:
  TileCache mPrevious;  ///< Previous tile state.
  TileCache mCurrent;   ///< The current stamp sequence.
  std::optional<TilePos> mLastChangedPos;
  bool mRandomMode{};

  void update_sequence(entt::registry& registry, const TilePos& cursor);

  void maybe_emit_event(entt::dispatcher& dispatcher);

  [[nodiscard]] auto is_usable(const entt::registry& registry) const -> bool;
};

}  // namespace tactile
