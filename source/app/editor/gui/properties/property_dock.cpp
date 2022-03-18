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

#include "property_dock.hpp"

#include <locale>   // locale, isalpha, isdigit, isspace
#include <utility>  // move

#include <imgui.h>

#include "core/components/attributes.hpp"
#include "core/components/layers.hpp"
#include "core/components/objects.hpp"
#include "core/components/tiles.hpp"
#include "core/map.hpp"
#include "core/systems/context_system.hpp"
#include "core/systems/property_system.hpp"
#include "core/systems/registry_system.hpp"
#include "dialogs/add_property_dialog.hpp"
#include "dialogs/change_property_type_dialog.hpp"
#include "dialogs/rename_property_dialog.hpp"
#include "editor/events/layer_events.hpp"
#include "editor/events/object_events.hpp"
#include "editor/events/property_events.hpp"
#include "editor/events/tileset_events.hpp"
#include "editor/gui/common/input_widgets.hpp"
#include "editor/gui/icons.hpp"
#include "editor/gui/properties/items/property_item_context_menu.hpp"
#include "editor/gui/scoped.hpp"
#include "editor/model.hpp"
#include "io/persistence/preferences.hpp"
#include "meta/build.hpp"
#include "tactile.hpp"

namespace tactile {
namespace {

inline PropertyItemContextMenuState _context_state;
inline Maybe<std::string> _rename_target;
inline Maybe<std::string> _change_type_target;
constinit bool _is_focused = false;

inline AddPropertyDialog _add_dialog;
inline RenamePropertyDialog _rename_dialog;
inline ChangePropertyTypeDialog _change_type_dialog;

void _prepare_table_row(const char* label)
{
  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted(label);
}

[[nodiscard]] auto _native_name_row(const std::string& name,
                                    const bool validateAsFileName = false)
    -> Maybe<std::string>
{
  _prepare_table_row("Name");

  ImGui::TableNextColumn();

  auto flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
  if (validateAsFileName) {
    flags |= ImGuiInputTextFlags_CallbackCharFilter;

    /* This is a basic filter for only allowing a basic subset of characters, in order to
       guarantee that the user provides names that are usable as file names. */
    const auto filter = [](ImGuiInputTextCallbackData* data) -> int {
      const auto& locale = std::locale::classic();
      const auto ch = static_cast<wchar_t>(data->EventChar);

      if (std::isalpha(ch, locale) || std::isdigit(ch, locale) ||
          std::isspace(ch, locale) || ch == '-' || ch == '_') {
        return 0;  // Accept the character
      }
      else {
        return 1;  // Reject the character
      }
    };

    return input_string("##NativeNameRowInput", name, nullptr, flags, filter);
  }
  else {
    return input_string("##NativeNameRowInput", name, nullptr, flags);
  }
}

[[nodiscard]] auto _native_opacity_row(const float opacity) -> Maybe<float>
{
  _prepare_table_row("Opacity");

  ImGui::TableNextColumn();
  return input_float("##_native_opacity_row", opacity, 0.0f, 1.0f);
}

[[nodiscard]] auto _native_visibility_row(const bool visible) -> Maybe<bool>
{
  _prepare_table_row("Visible");

  ImGui::TableNextColumn();
  return input_bool("##_native_visibility_row", visible);
}

void _native_read_only_row(const char* label, const char* value)
{
  _prepare_table_row(label);

  ImGui::TableNextColumn();
  ImGui::TextUnformatted(value);
}

void _native_read_only_row(const char* label, const float value)
{
  _prepare_table_row(label);

  ImGui::TableNextColumn();
  ImGui::Text("%.2f", value);
}

void _native_read_only_row(const char* label, const int32 value)
{
  _prepare_table_row(label);

  ImGui::TableNextColumn();
  ImGui::Text("%d", value);
}

void _native_read_only_row(const char* label, const usize value)
{
  _prepare_table_row(label);

  ImGui::TableNextColumn();
  ImGui::Text("%llu", static_cast<ulonglong>(value)); /* Cast to avoid format warnings */
}

void _show_native_map_properties(const std::string& name, const MapInfo& map)
{
  _native_read_only_row("Type", "Map");
  _native_read_only_row("Name", name.c_str());

  _native_read_only_row("Tile width", map.tile_width);
  _native_read_only_row("Tile height", map.tile_height);

  _native_read_only_row("Row count", map.row_count);
  _native_read_only_row("Column count", map.column_count);
}

void _show_native_tileset_properties(const std::string& name,
                                     const comp::Tileset& tileset,
                                     entt::dispatcher& dispatcher)
{
  _native_read_only_row("Type", "Tileset");

  if constexpr (is_debug_build) {
    _native_read_only_row("ID", tileset.id);
  }

  if (const auto updatedName = _native_name_row(name, true);
      updatedName && !updatedName->empty()) {
    dispatcher.enqueue<SetTilesetNameEvent>(tileset.id, *updatedName);
  }

  _native_read_only_row("First tile ID", tileset.first_id);
  _native_read_only_row("Last tile ID", tileset.last_id);

  _native_read_only_row("Tile count", tileset.tile_count);
  _native_read_only_row("Column count", tileset.column_count);

  _native_read_only_row("Tile width", tileset.tile_width);
  _native_read_only_row("Tile height", tileset.tile_height);
}

void _show_native_layer_properties(const comp::Layer& layer, entt::dispatcher& dispatcher)
{
  switch (layer.type) {
    case LayerType::tile_layer:
      _native_read_only_row("Type", "Tile Layer");
      break;

    case LayerType::object_layer:
      _native_read_only_row("Type", "Object Layer");
      break;

    case LayerType::group_layer:
      _native_read_only_row("Type", "Group Layer");
      break;
  }

  if constexpr (is_debug_build) {
    _native_read_only_row("ID", layer.id);
  }

  if (const auto value = _native_opacity_row(layer.opacity)) {
    dispatcher.enqueue<SetLayerOpacityEvent>(layer.id, *value);
  }

  if (const auto value = _native_visibility_row(layer.visible)) {
    dispatcher.enqueue<SetLayerVisibleEvent>(layer.id, *value);
  }
}

void _show_native_object_properties(const std::string& name,
                                    const comp::Object& object,
                                    entt::dispatcher& dispatcher)
{
  switch (object.type) {
    case ObjectType::rect:
      _native_read_only_row("Type", "Rectangle");
      break;

    case ObjectType::point:
      _native_read_only_row("Type", "Point");
      break;

    case ObjectType::ellipse:
      _native_read_only_row("Type", "Ellipse");
      break;
  }

  if constexpr (is_debug_build) {
    _native_read_only_row("ID", object.id);
  }

  if (const auto updatedName = _native_name_row(name)) {
    dispatcher.enqueue<SetObjectNameEvent>(object.id, *updatedName);
  }

  _native_read_only_row("X", object.x);
  _native_read_only_row("Y", object.y);

  if (object.type != ObjectType::point) {
    _native_read_only_row("Width", object.width);
    _native_read_only_row("Height", object.height);
  }

  if (const auto visible = _native_visibility_row(object.visible)) {
    dispatcher.enqueue<SetObjectVisibilityEvent>(object.id, *visible);
  }

  _prepare_table_row("Tag");

  ImGui::TableNextColumn();
  if (const auto tag = input_string("##NativeObjectTagInput", object.tag)) {
    dispatcher.enqueue<SetObjectTagEvent>(object.id, *tag);
  }
}

void show_custom_properties(const entt::registry& registry,
                            entt::dispatcher& dispatcher,
                            const comp::AttributeContext& context,
                            bool& isItemContextOpen)
{
  bool first = true;

  for (const auto entity : context.properties) {
    const auto& property = sys::checked_get<comp::Property>(registry, entity);

    const auto& name = property.name;
    const auto& value = property.value;

    const scoped::Id scope{name.c_str()};

    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    if (first) {
      ImGui::Separator();
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Selectable(name.c_str());

    if (!isItemContextOpen) {
      isItemContextOpen = property_item_context_menu(dispatcher, name, _context_state);
    }

    if (_context_state.show_rename_dialog && !_rename_target) {
      _rename_target = name;
    }

    if (_context_state.show_change_type_dialog && !_change_type_target) {
      _change_type_target = name;
    }

    ImGui::TableNextColumn();

    if (first) {
      ImGui::Separator();
    }

    if (auto updated = input_attribute("##CustomPropertyInput", value)) {
      dispatcher.enqueue<UpdatePropertyEvent>(name, std::move(*updated));
    }

    first = false;
  }
}

void _update_property_table(const entt::registry& registry, entt::dispatcher& dispatcher)
{
  constexpr auto flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
                         ImGuiTableFlags_ScrollY | ImGuiTableFlags_PadOuterX;

  const auto& current = registry.ctx<comp::ActiveAttributeContext>();
  const auto& context = sys::current_context(registry);

  if (scoped::Table table{"##PropertyTable", 2, flags}; table.is_open()) {
    if (current.entity == entt::null) {
      _show_native_map_properties(context.name, registry.ctx<MapInfo>());
    }
    else {
      if (const auto* tileset = registry.try_get<comp::Tileset>(current.entity)) {
        _show_native_tileset_properties(context.name, *tileset, dispatcher);
      }
      else if (const auto* layer = registry.try_get<comp::Layer>(current.entity)) {
        _show_native_layer_properties(*layer, dispatcher);
      }
      else if (const auto* object = registry.try_get<comp::Object>(current.entity)) {
        _show_native_object_properties(context.name, *object, dispatcher);
      }
    }

    bool isItemContextOpen = false;
    show_custom_properties(registry, dispatcher, context, isItemContextOpen);

    if (!isItemContextOpen) {
      if (auto popup = scoped::Popup::for_window("##PropertyTableContext");
          popup.is_open()) {
        _context_state.show_add_dialog =
            ImGui::MenuItem(TAC_ICON_ADD " Add New Property...");
      }
    }
  }

  if (_context_state.show_add_dialog) {
    dispatcher.enqueue<ShowAddPropertyDialogEvent>();
    _context_state.show_add_dialog = false;
  }

  if (_context_state.show_rename_dialog) {
    dispatcher.enqueue<ShowRenamePropertyDialogEvent>(_rename_target.value());
    _rename_target.reset();
    _context_state.show_rename_dialog = false;
  }

  if (_context_state.show_change_type_dialog) {
    const auto& name = _change_type_target.value();
    const auto type = sys::get_property(registry, context, name).value.type();
    dispatcher.enqueue<ShowChangePropertyTypeDialogEvent>(name, type);
    _change_type_target.reset();
    _context_state.show_change_type_dialog = false;
  }
}

}  // namespace

void update_property_dock(const DocumentModel& model, entt::dispatcher& dispatcher)
{
  auto& prefs = get_preferences();

  bool visible = prefs.is_property_dock_visible();
  if (!visible) {
    return;
  }

  constexpr auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

  scoped::Window window{"Properties", flags, &visible};
  _is_focused = window.is_open() && window.has_focus();

  if (window.is_open()) {
    const auto& registry = model.get_active_registry();
    _update_property_table(registry, dispatcher);

    _add_dialog.update(model, dispatcher);
    _rename_dialog.update(model, dispatcher);
    _change_type_dialog.update(model, dispatcher);
  }

  prefs.set_property_dock_visible(visible);
}

void show_add_property_dialog()
{
  _add_dialog.open();
}

void show_rename_property_dialog(const std::string& name)
{
  _rename_dialog.show(name);
}

void show_change_property_type_dialog(std::string name, const AttributeType type)
{
  _change_type_dialog.show(std::move(name), type);
}

auto is_property_dock_focused() -> bool
{
  return _is_focused;
}

}  // namespace tactile
