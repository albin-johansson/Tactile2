// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/ui/menu/file_menu.hpp"

#include <imgui.h>

#include "tactile/core/event/dialog_events.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/file_events.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/ui/common/menus.hpp"
#include "tactile/core/ui/i18n/language.hpp"
#include "tactile/core/ui/shortcuts.hpp"

namespace tactile::ui {
inline namespace file_menu {

void _push_recent_files_menu(const Language& language,
                             EventDispatcher& dispatcher)
{
  const MenuScope menu {language.get(StringID::kRecentFilesMenu)};
  if (menu.is_open()) {
    if (ImGui::MenuItem(language.get(StringID::kReopenLastClosedFile),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<ReopenLastClosedFileEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(StringID::kClearFileHistory),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<ClearFileHistoryEvent>();
    }
  }
}

}  // namespace file_menu

void FileMenu::push(const Model& model, EventDispatcher& dispatcher)
{
  const auto& language = model.get_language();
  const auto* current_document = model.get_current_document();

  const MenuScope menu {language.get(StringID::kFileMenu)};
  if (menu.is_open()) {
    if (ImGui::MenuItem(language.get(StringID::kCreateMap),
                        kCreateMapShortcut.hint)) {
      dispatcher.push<ShowNewMapDialogEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(StringID::kOpen), kOpenShortcut.hint)) {
      dispatcher.push<ShowOpenMapDialogEvent>();
    }

    _push_recent_files_menu(language, dispatcher);

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(StringID::kSave),
                        kSaveShortcut.hint,
                        false,
                        false)) {
      dispatcher.push<SaveEvent>();
    }

    if (ImGui::MenuItem(language.get(StringID::kSaveAs),
                        kSaveAsShortcut.hint,
                        false,
                        current_document != nullptr)) {
      dispatcher.push<ShowSaveAsDialogEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(StringID::kClose),
                        nullptr,
                        false,
                        current_document != nullptr)) {
      dispatcher.push<CloseEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(StringID::kQuit))) {
      dispatcher.push<QuitEvent>();
    }
  }
}

}  // namespace tactile::ui