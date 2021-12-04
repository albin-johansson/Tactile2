#include "document_tabs.hpp"

#include <imgui.h>

#include "core/components/property_context.hpp"
#include "editor/events/map_events.hpp"
#include "editor/gui/scoped.hpp"
#include "editor/model.hpp"
#include "map_view.hpp"

namespace Tactile {

void UpdateDocumentTabs(const Model& model, entt::dispatcher& dispatcher)
{
  if (Scoped::TabBar bar{"##MapViewportTabBar", ImGuiTabBarFlags_Reorderable};
      bar.IsOpen()) {
    for (const auto& [id, document] : model) {
      const Scoped::ID scope{id};

      ImGuiTabItemFlags flags = 0;
      const auto isActive = model.GetActiveMapId() == id;

      if (isActive) {
        flags |= ImGuiTabItemFlags_SetSelected;

        if (!model.IsClean()) {
          flags |= ImGuiTabItemFlags_UnsavedDocument;
        }
      }

      const auto& context = document->registry.ctx<PropertyContext>();
      bool opened = true;
      if (Scoped::TabItem item{context.name.c_str(), &opened, flags}; item.IsOpen()) {
        if (isActive) {
          UpdateMapView(document->registry, dispatcher);
        }
      }

      if (!opened) {
        dispatcher.enqueue<CloseMapEvent>(id);
      }
      else if (ImGui::IsItemActivated()) {
        dispatcher.enqueue<SelectMapEvent>(id);
      }
    }
  }
}

}  // namespace Tactile
