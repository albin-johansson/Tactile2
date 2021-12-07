#pragma once

#include <entt/entt.hpp>

#include "editor/gui/dialogs/create_map_dialog.hpp"

namespace Tactile {

class Model;

/// \addtogroup gui
/// \{

class FileMenu final {
 public:
  void Update(const Model& model, entt::dispatcher& dispatcher);

  /* Updates all associated windows. */
  void UpdateWindows(const entt::registry& registry, entt::dispatcher& dispatcher);

  void ShowNewMapDialog();

  void ShowOpenMapDialog();

 private:
  CreateMapDialog mCreateMapDialog;
  bool mShowOpenMapDialog{};

  void UpdateRecentFilesMenu(entt::dispatcher& dispatcher);
  void UpdateMapFileDialog(entt::dispatcher& dispatcher);
};

/// \} End of group gui

}  // namespace Tactile
