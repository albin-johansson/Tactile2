// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/ui/menu/edit_menu.hpp"

#include <imgui.h>

#include "tactile/core/event/dialog_events.hpp"
#include "tactile/core/event/edit_events.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/ui/common/menus.hpp"
#include "tactile/core/ui/i18n/language.hpp"
#include "tactile/core/ui/shortcuts.hpp"

namespace tactile::ui {

void EditMenu::push(const Model& model, EventDispatcher& dispatcher)
{
  const auto& language = model.get_language();

  const MenuScope menu {language.get(StringID::kEditMenu)};
  if (menu.is_open()) {
    if (ImGui::MenuItem(language.get(StringID::kUndo),
                        kUndoShortcut.hint,
                        false,
                        false)) {
      dispatcher.push<UndoEvent>();
    }

    if (ImGui::MenuItem(language.get(StringID::kRedo),
                        kRedoShortcut.hint,
                        false,
                        false)) {
      dispatcher.push<RedoEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(StringID::kStampTool),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<EnableStampToolEvent>();
    }

    if (ImGui::MenuItem(language.get(StringID::kEraserTool),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<EnableEraserToolEvent>();
    }

    if (ImGui::MenuItem(language.get(StringID::kBucketTool),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<EnableBucketToolEvent>();
    }

    if (ImGui::MenuItem(language.get(StringID::kObjectSelectorTool),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<EnableSelectorToolEvent>();
    }

    if (ImGui::MenuItem(language.get(StringID::kRectangleTool),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<EnableRectangleToolEvent>();
    }

    if (ImGui::MenuItem(language.get(StringID::kEllipseTool),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<EnableEllipseToolEvent>();
    }

    if (ImGui::MenuItem(language.get(StringID::kPointTool),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<EnablePointToolEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(StringID::kOpenComponentEditor),
                        kOpenComponentEditorShortcut.hint,
                        false,
                        false)) {
      dispatcher.push<ShowComponentEditorDialogEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(StringID::kOpenSettings),
                        kOpenSettingsShortcut.hint)) {
      dispatcher.push<ShowSettingsDialogEvent>();
    }
  }
}

}  // namespace tactile::ui