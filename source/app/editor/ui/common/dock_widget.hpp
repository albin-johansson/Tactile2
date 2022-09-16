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
#include <imgui.h>

#include "core/common/macros.hpp"
#include "core/common/memory.hpp"

namespace tactile {
class DocumentModel;
}  // namespace tactile

namespace tactile::ui {

class DockWidget {
 public:
  TACTILE_DELETE_COPY(DockWidget);
  TACTILE_DEFAULT_MOVE(DockWidget);

  DockWidget(const char* title, ImGuiWindowFlags flags);

  virtual ~DockWidget() noexcept;

  void update(const DocumentModel& model, entt::dispatcher& dispatcher);

  [[nodiscard]] auto has_focus() const -> bool;

  [[nodiscard]] auto has_mouse_hover() const -> bool;

 protected:
  virtual void on_update([[maybe_unused]] const DocumentModel& model,
                         [[maybe_unused]] entt::dispatcher& dispatcher)
  {}

  void set_close_button_enabled(bool enabled);

  void set_focus_flags(ImGuiFocusedFlags flags);

  virtual void set_visible(bool visible) = 0;

  [[nodiscard]] virtual auto is_visible() const -> bool = 0;

 private:
  struct Data;
  Unique<Data> mData;
};

}  // namespace tactile::ui