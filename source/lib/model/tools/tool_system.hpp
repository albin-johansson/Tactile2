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

#include "common/enum/tool_type.hpp"
#include "common/macros.hpp"
#include "common/type/hash_map.hpp"
#include "common/type/maybe.hpp"
#include "common/type/ptr.hpp"
#include "model/system.hpp"

namespace tactile {

TACTILE_FWD_DECLARE_CLASS(Tool)
TACTILE_FWD_DECLARE_STRUCT(SelectToolEvent)
TACTILE_FWD_DECLARE_STRUCT(ViewportMousePressedEvent)
TACTILE_FWD_DECLARE_STRUCT(ViewportMouseDraggedEvent)
TACTILE_FWD_DECLARE_STRUCT(ViewportMouseReleasedEvent)
TACTILE_FWD_DECLARE_STRUCT(ViewportMouseEnteredEvent)
TACTILE_FWD_DECLARE_STRUCT(ViewportMouseExitedEvent)

class ToolSystem final : public System {
 public:
  ToolSystem(Model& model, Dispatcher& dispatcher);

  ~ToolSystem() noexcept override;

  void reset() override;

  void install() override;

  [[nodiscard]] auto is_tool_available(ToolType type) const -> bool;

 private:
  HashMap<ToolType, Unique<Tool>> mTools;
  Tool* mActiveTool {};
  Maybe<ToolType> mActiveToolType;

  void _on_select_tool(const SelectToolEvent& event);
  void _on_viewport_mouse_pressed(const ViewportMousePressedEvent& event);
  void _on_viewport_mouse_dragged(const ViewportMouseDraggedEvent& event);
  void _on_viewport_mouse_released(const ViewportMouseReleasedEvent& event);
  void _on_viewport_mouse_entered(const ViewportMouseEnteredEvent& event);
  void _on_viewport_mouse_exited(const ViewportMouseExitedEvent& event);
};

}  // namespace tactile