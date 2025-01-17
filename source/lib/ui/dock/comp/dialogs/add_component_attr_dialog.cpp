// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "add_component_attr_dialog.hpp"

#include <entt/signal/dispatcher.hpp>
#include <imgui.h>

#include "common/util/string_buffer.hpp"
#include "core/component/component_index.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/event/component_events.hpp"
#include "model/model.hpp"
#include "ui/dialog/dialog.hpp"

namespace tactile::ui {
namespace {

struct AddComponentAttrDialogState final {
  Maybe<UUID> component_id;
  StringBuffer attribute_name_buffer {};
  bool open_dialog {};
};

inline AddComponentAttrDialogState gDialogState;

}  // namespace

void open_create_component_attribute_dialog(const UUID& component_id)
{
  gDialogState.component_id = component_id;
  gDialogState.attribute_name_buffer.clear();
  gDialogState.open_dialog = true;
}

void update_create_component_attribute_dialog(const DocumentModel& model,
                                              entt::dispatcher& dispatcher)
{
  const auto& lang = get_current_language();
  const auto* component_index = model.require_active_document().find_component_index();

  if (gDialogState.component_id.has_value() &&  //
      component_index != nullptr &&
      !component_index->has_comp(*gDialogState.component_id)) {
    gDialogState.component_id.reset();
    gDialogState.open_dialog = false;
    return;
  }

  DialogOptions options {
      .title = lang.window.create_attribute.c_str(),
      .close_label = lang.misc.cancel.c_str(),
      .accept_label = lang.misc.create.c_str(),
  };

  if (gDialogState.open_dialog) {
    options.flags |= UI_DIALOG_FLAG_OPEN;
    gDialogState.open_dialog = false;
  }

  const auto current_name = gDialogState.attribute_name_buffer.as_string_view();

  if (!current_name.empty() &&  //
      component_index != nullptr &&
      !component_index->get_comp(gDialogState.component_id.value())
           .has_attr(current_name)) {
    options.flags |= UI_DIALOG_FLAG_INPUT_IS_VALID;
  }

  DialogAction action {DialogAction::None};
  if (const ScopedDialog dialog {options, &action}; dialog.was_opened()) {
    ImGui::InputTextWithHint("##Name",
                             lang.misc.attribute_name_hint.c_str(),
                             gDialogState.attribute_name_buffer.data(),
                             sizeof gDialogState.attribute_name_buffer);
  }

  if (action == DialogAction::Accept) {
    dispatcher.enqueue<AddComponentAttrEvent>(
        gDialogState.component_id.value(),
        gDialogState.attribute_name_buffer.as_string());
  }
}

}  // namespace tactile::ui
