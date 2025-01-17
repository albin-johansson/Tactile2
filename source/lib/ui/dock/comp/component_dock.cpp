// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "component_dock.hpp"

#include <entt/signal/dispatcher.hpp>
#include <imgui.h>

#include "common/util/fmt.hpp"
#include "component_view.hpp"
#include "core/component/component_index.hpp"
#include "core/context/context.hpp"
#include "core/context/context_info.hpp"
#include "core/context/context_manager.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/event/component_events.hpp"
#include "model/model.hpp"
#include "model/settings.hpp"
#include "tactile/core/debug/assert.hpp"
#include "ui/style/alignment.hpp"
#include "ui/style/icons.hpp"
#include "ui/widget/scoped.hpp"
#include "ui/widget/widgets.hpp"

namespace tactile::ui {
namespace {

inline constexpr auto kAddComponentPopupId = "##AddComponentButtonPopup";

void show_add_component_button_popup_content(const Document& document,
                                             const Context& context,
                                             entt::dispatcher& dispatcher)
{
  const auto& lang = get_current_language();
  const auto* component_index = document.find_component_index();
  TACTILE_ASSERT(component_index != nullptr);

  if (component_index->empty()) {
    const Disable disable;
    ImGui::TextUnformatted(lang.misc.no_available_components.c_str());
  }
  else {
    const auto& ctx = context.get_ctx();
    for (const auto& [component_id, component_def]: *component_index) {
      const Disable disable_if {ctx.has_component(component_id)};
      if (ImGui::MenuItem(component_def.get_name().c_str())) {
        dispatcher.enqueue<AttachComponentEvent>(context.get_uuid(), component_id);
      }
    }
  }

  ImGui::Separator();
  if (ImGui::MenuItem(lang.action.component_editor.c_str())) {
    dispatcher.enqueue<OpenComponentEditorEvent>();
  }
}

void show_contents(const Document& document, entt::dispatcher& dispatcher)
{
  const auto& lang = get_current_language();
  const auto& context = document.get_contexts().get_active_context();

  const FmtString indicator {"{}: {}", lang.misc.context, context.get_ctx().name()};
  ImGui::TextUnformatted(indicator.data());

  if (const Child pane {"##ComponentsChild"}; pane.is_open()) {
    const auto& ctx = context.get_ctx();
    if (ctx.component_count() == 0) {
      prepare_vertical_alignment_center(2);
      ui_centered_label(lang.misc.context_has_no_components.c_str());
    }
    else {
      const auto* component_index = document.find_component_index();
      TACTILE_ASSERT(component_index != nullptr);

      ctx.each_component([&](const UUID& component_id, const Component& component) {
        ImGui::Separator();

        const auto& component_name = component_index->get_comp(component_id).get_name();
        component_view(context.get_uuid(), component, component_name, dispatcher);
      });

      ImGui::Separator();
    }

    if (ui_centered_button(TAC_ICON_ADD, lang.tooltip.add_component.c_str())) {
      ImGui::OpenPopup(kAddComponentPopupId);
    }

    if (const Popup popup {kAddComponentPopupId}; popup.is_open()) {
      show_add_component_button_popup_content(document, context, dispatcher);
    }
  }
}

}  // namespace

void update_component_dock(const DocumentModel& model, entt::dispatcher& dispatcher)
{
  auto& settings = get_settings();

  if (!settings.test_flag(SETTINGS_SHOW_COMPONENT_DOCK_BIT)) {
    return;
  }

  const auto& lang = get_current_language();

  bool show_component_dock = true;
  const Window dock {lang.window.component_dock.c_str(),
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar,
                     &show_component_dock};
  settings.set_flag(SETTINGS_SHOW_COMPONENT_DOCK_BIT, show_component_dock);

  if (dock.is_open()) {
    const auto& document = model.require_active_document();
    show_contents(document, dispatcher);
  }
}

}  // namespace tactile::ui
