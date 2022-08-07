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

#include "core/common/macros.hpp"
#include "editor/ui/dialogs/dialog.hpp"
#include "io/persist/preferences.hpp"

namespace tactile::ui {

/// Provides the interface for all available persistent settings.
class SettingsDialog final : public ADialog
{
 public:
  TACTILE_DEFAULT_COPY(SettingsDialog);
  TACTILE_DEFAULT_MOVE(SettingsDialog);

  SettingsDialog();

  ~SettingsDialog() override = default;

  void show();

 protected:
  void on_update(const DocumentModel& model, entt::dispatcher& dispatcher) override;

  void on_cancel() override;

  void on_accept(entt::dispatcher& dispatcher) override;

  void on_apply(entt::dispatcher& dispatcher) override;

 private:
  io::PreferenceState mSnapshot;    /// Initial settings when the dialog was opened.
  io::PreferenceState mUiSettings;  /// State of settings in the UI.

  void apply_settings(entt::dispatcher& dispatcher);

  void update_behavior_tab();

  void update_appearance_tab();

  void update_export_tab();
};

}  // namespace tactile::ui
