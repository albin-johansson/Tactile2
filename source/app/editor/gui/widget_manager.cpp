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

#include "widget_manager.hpp"

#include <utility>  // move

#include "components/component_dock.hpp"
#include "dialogs/map_parse_error_dialog.hpp"
#include "dialogs/resize_map_dialog.hpp"
#include "editor/model.hpp"
#include "icons.hpp"
#include "layers/layer_dock.hpp"
#include "layout/dock_space.hpp"
#include "log/log_dock.hpp"
#include "menus/menu_bar.hpp"
#include "properties/property_dock.hpp"
#include "tilesets/tileset_dock.hpp"
#include "tilesets/tileset_view.hpp"
#include "toolbar/toolbar.hpp"
#include "viewport/map_view.hpp"
#include "viewport/viewport_widget.hpp"

namespace tactile {

struct WidgetManager::Widgets final
{
  MenuBar menu_bar;
  Toolbar toolbar;
  TilesetDock tileset_dock;
  LayerDock layer_dock;
  PropertyDock property_dock;
  ComponentDock component_dock;
  LogDock log_dock;
  ResizeMapDialog resize_map_dialog;
  MapParseErrorDialog map_parse_error_dialog;
};

WidgetManager::WidgetManager() : mWidgets{std::make_unique<Widgets>()} {}

WidgetManager::~WidgetManager() noexcept = default;

void WidgetManager::update(const DocumentModel& model,
                           const IconManager& icons,
                           entt::dispatcher& dispatcher)
{
  mWidgets->menu_bar.update(model, dispatcher);
  update_dock_space();

  if (model.has_active_document()) {
    mWidgets->toolbar.Update(model, dispatcher);
    mWidgets->layer_dock.update(model, dispatcher);
    mWidgets->tileset_dock.update(model, dispatcher);
    mWidgets->property_dock.update(model, dispatcher);
    mWidgets->component_dock.update(model, dispatcher);
    mWidgets->log_dock.update(model, dispatcher);
  }

  update_viewport_widget(model, icons, dispatcher);

  if (const auto* registry = model.active_registry()) {
    update_map_view_object_context_menu(*registry, dispatcher);
  }

  mWidgets->resize_map_dialog.update(model, dispatcher);
  mWidgets->map_parse_error_dialog.update(model, dispatcher);
}

void WidgetManager::show_settings()
{
  mWidgets->menu_bar.show_settings_dialog();
}

void WidgetManager::show_new_map_dialog()
{
  mWidgets->menu_bar.show_map_creation_dialog();
}

void WidgetManager::show_open_map_dialog()
{
  mWidgets->menu_bar.show_open_map_dialog();
}

void WidgetManager::show_add_tileset_dialog()
{
  mWidgets->menu_bar.show_tileset_creation_dialog();
}

void WidgetManager::show_rename_layer_dialog(const layer_id id)
{
  mWidgets->layer_dock.show_rename_layer_dialog(id);
}

void WidgetManager::show_add_property_dialog()
{
  mWidgets->property_dock.show_add_property_dialog();
}

void WidgetManager::show_rename_property_dialog(const std::string& name)
{
  mWidgets->property_dock.show_rename_property_dialog(name);
}

void WidgetManager::show_change_property_type_dialog(std::string name,
                                                     const AttributeType type)
{
  mWidgets->property_dock.show_change_property_type_dialog(std::move(name), type);
}

void WidgetManager::show_resize_map_dialog(const usize currentRows,
                                           const usize currentColumns)
{
  mWidgets->resize_map_dialog.show(currentRows, currentColumns);
}

void WidgetManager::show_map_import_error_dialog(const parsing::ParseError error)
{
  mWidgets->map_parse_error_dialog.show(error);
}

void WidgetManager::show_component_editor(const DocumentModel& model)
{
  mWidgets->menu_bar.show_component_editor(model);
}

void WidgetManager::set_toolbar_visible(const bool visible)
{
  mWidgets->toolbar.SetVisible(visible);
}

auto WidgetManager::is_editor_focused() const -> bool
{
  return is_toolbar_focused() || is_viewport_focused() || is_layer_dock_focused() ||
         is_tileset_dock_focused() || is_property_dock_focused() || is_log_dock_focused();
}

auto WidgetManager::is_toolbar_focused() const -> bool
{
  return mWidgets->toolbar.IsFocused();
}

auto WidgetManager::is_viewport_focused() const -> bool
{
  return tactile::is_viewport_focused();
}

auto WidgetManager::is_layer_dock_focused() const -> bool
{
  return mWidgets->layer_dock.has_focus();
}

auto WidgetManager::is_tileset_dock_focused() const -> bool
{
  return mWidgets->tileset_dock.has_focus();
}

auto WidgetManager::is_property_dock_focused() const -> bool
{
  return mWidgets->property_dock.has_focus();
}

auto WidgetManager::is_log_dock_focused() const -> bool
{
  return mWidgets->log_dock.has_focus();
}

auto WidgetManager::is_tileset_dock_hovered() const -> bool
{
  return mWidgets->tileset_dock.has_mouse_hover();
}

auto WidgetManager::is_toolbar_visible() const -> bool
{
  return mWidgets->toolbar.IsVisible();
}

auto WidgetManager::tileset_view_width() const -> Maybe<float>
{
  return mWidgets->tileset_dock.get_tileset_view().width();
}

auto WidgetManager::tileset_view_height() const -> Maybe<float>
{
  return mWidgets->tileset_dock.get_tileset_view().height();
}

}  // namespace tactile