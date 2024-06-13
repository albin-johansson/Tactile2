// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/ui/widget_manager.hpp"

#include "tactile/core/document/map_document.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/ui/shortcuts.hpp"

namespace tactile::ui {

void WidgetManager::push(const Model& model, EventDispatcher& dispatcher)
{
  const auto& language = model.get_language();
  const auto* current_doc = model.get_current_document();
  const auto* current_map_doc = dynamic_cast<const MapDocument*>(current_doc);

  mMenuBar.push(model, dispatcher);
  mDockSpace.update(language);
  mDocumentDock.push(model, dispatcher);

  if (current_map_doc != nullptr) {
    mTilesetDock.push(language, *current_map_doc, dispatcher);
    mLayerDock.update(language, *current_map_doc, dispatcher);
  }

  if (current_doc != nullptr) {
    mPropertyDock.push(language, *current_doc, dispatcher);
    mComponentDock.push(language, *current_doc, dispatcher);
    mLogDock.push(model, dispatcher);
  }

  mNewMapDialog.push(model, dispatcher);

  push_global_shortcuts(model, dispatcher);
}

auto WidgetManager::get_dock_space() -> DockSpace&
{
  return mDockSpace;
}

auto WidgetManager::get_new_map_dialog() -> NewMapDialog&
{
  return mNewMapDialog;
}

}  // namespace tactile::ui