#include "file_menu.hpp"

#include <imgui.h>

#include "editor/events/map_events.hpp"
#include "editor/events/misc_events.hpp"
#include "editor/gui/icons.hpp"
#include "editor/gui/scoped.hpp"
#include "editor/model.hpp"
#include "editor/shortcuts/mappings.hpp"
#include "io/file_dialog.hpp"
#include "io/persistence/history.hpp"

namespace tactile {

void FileMenu::Update(const Model& model, entt::dispatcher& dispatcher)
{
  if (scoped::menu menu{"File"}; menu.is_open()) {
    const auto hasActiveDocument = model.has_active_document();

    if (ImGui::MenuItem(TAC_ICON_FILE " Create Map...", TACTILE_PRIMARY_MOD "+N")) {
      mCreateMapDialog.Open();
    }

    mShowOpenMapDialog =
        ImGui::MenuItem(TAC_ICON_OPEN " Open Map...", TACTILE_PRIMARY_MOD "+O");

    UpdateRecentFilesMenu(dispatcher);

    ImGui::Separator();

    if (ImGui::MenuItem(TAC_ICON_SAVE " Save",
                        TACTILE_PRIMARY_MOD "+S",
                        false,
                        model.is_save_possible())) {
      dispatcher.enqueue<save_event>();
    }

    if (ImGui::MenuItem(TAC_ICON_SAVE " Save As...",
                        TACTILE_PRIMARY_MOD "+Shift+S",
                        false,
                        hasActiveDocument)) {
      dispatcher.enqueue<open_save_as_dialog_event>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(TAC_ICON_CLOSE " Close Map", nullptr, false, hasActiveDocument)) {
      dispatcher.enqueue<close_map_event>(model.active_map_id().value());
    }

    ImGui::Separator();

    if (ImGui::MenuItem(TAC_ICON_EXIT " Exit")) {
      dispatcher.enqueue<quit_event>();
    }
  }
}

void FileMenu::UpdateWindows(const Model& model, entt::dispatcher& dispatcher)
{
  mCreateMapDialog.update(model, dispatcher);

  if (mShowOpenMapDialog) {
    UpdateMapFileDialog(dispatcher);
  }
}

void FileMenu::ShowNewMapDialog()
{
  mCreateMapDialog.Open();
}

void FileMenu::ShowOpenMapDialog()
{
  mShowOpenMapDialog = true;
}

void FileMenu::UpdateRecentFilesMenu(entt::dispatcher& dispatcher)
{
  if (scoped::menu menu{TAC_ICON_HISTORY " Recent Files"}; menu.is_open()) {
    if (ImGui::MenuItem(TAC_ICON_OPEN " Reopen Last Closed File",
                        nullptr,
                        false,
                        is_last_closed_file_valid())) {
      // TODO this will need to be tweaked if tileset documents viewing will be supported
      dispatcher.enqueue<open_map_event>(last_closed_file());
    }

    const auto& history = file_history();

    if (!history.empty()) {
      ImGui::Separator();
    }

    for (const auto& path : history) {
      if (ImGui::MenuItem(path.c_str())) {
        /* It's fine if the file doesn't exist anymore, the parser handles that */
        dispatcher.enqueue<open_map_event>(path);
      }
    }

    ImGui::Separator();

    if (ImGui::MenuItem(TAC_ICON_CLEAR_HISTORY " Clear File History",
                        nullptr,
                        false,
                        !history.empty())) {
      clear_file_history();
    }
  }
}

void FileMenu::UpdateMapFileDialog(entt::dispatcher& dispatcher)
{
  auto dialog = file_dialog::open_map();

  if (dialog.is_okay()) {
    dispatcher.enqueue<open_map_event>(dialog.path());
  }

  mShowOpenMapDialog = false;
}

}  // namespace tactile
