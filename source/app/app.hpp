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

#pragma once

#include <centurion/event.hpp>
#include <entt/signal/dispatcher.hpp>

#include "core/event/all.hpp"
#include "core/model.hpp"
#include "core/util/texture_manager.hpp"
#include "editor/loop.hpp"

namespace tactile {

class Document;
class MapDocument;
class TilesetDocument;
class AppCfg;

/// Tracks visibility of widgets for the "Toggle UI" feature.
struct WidgetShowState final
{
  bool prev_show_layer_dock     : 1 {};
  bool prev_show_tileset_dock   : 1 {};
  bool prev_show_property_dock  : 1 {};
  bool prev_show_log_dock       : 1 {};
  bool prev_show_component_dock : 1 {};
};

/// The heart of the Tactile map editor.
class App final : EventLoop {
 public:
  explicit App(AppCfg* configuration);

  using EventLoop::start;

 protected:
  void on_startup() override;

  void on_shutdown() override;

  void on_pre_update() override;

  void on_update() override;

  void on_event(const cen::event_handler& handler) override;

 private:
  AppCfg* mConfig {}; /* Non-owning */
  entt::dispatcher mDispatcher;
  DocumentModel mModel;
  TextureManager mTextures;
  WidgetShowState mWidgetShowState;
  bool mReloadFonts : 1 {};

  [[nodiscard]] auto active_document() -> Document*;
  [[nodiscard]] auto active_map_document() -> MapDocument*;
  [[nodiscard]] auto active_tileset_document() -> TilesetDocument*;

  void subscribe_to_events();

  void save_current_files_to_history();

  void on_keyboard_event(cen::keyboard_event event);

  void on_mouse_wheel_event(const cen::mouse_wheel_event& event);

  void on_undo();
  void on_redo();

  void on_set_command_capacity(const SetCommandCapacityEvent& event);

  void on_save();
  void on_save_as(const SaveAsEvent& event);

  void on_open_save_as_dialog();

  void on_inspect_map();
  void on_inspect_tileset();

  void on_export_as_godot_scene(const ExportAsGodotSceneEvent& event);

  void on_create_map(const CreateMapEvent& event);

  void on_close_document(const CloseDocumentEvent& event);
  void on_open_document(const OpenDocumentEvent& event);

  void on_open_map(const OpenMapEvent& event);

  void on_select_document(const SelectDocumentEvent& event);

  void on_select_tool(const SelectToolEvent& event);

  void on_tool_pressed(const ToolPressedEvent& event);
  void on_tool_dragged(const ToolDraggedEvent& event);
  void on_tool_released(const ToolReleasedEvent& event);
  void on_tool_entered();
  void on_tool_exited();

  void on_stamp_sequence(StampSequenceEvent event);
  void on_set_stamp_randomizer_event(const SetStampRandomizerEvent& event);
  void on_eraser_sequence(EraserSequenceEvent event);
  void on_flood(const FloodEvent& event);

  void on_add_rectangle(const AddRectangleEvent& event);
  void on_add_ellipse(const AddEllipseEvent& event);
  void on_add_point(const AddPointEvent& event);

  void on_update_tileset_viewport_limits(const UpdateTilesetViewportLimitsEvent& event);

  void on_offset_document_viewport(const OffsetDocumentViewportEvent& event);

  void on_offset_tileset_viewport(const OffsetTilesetViewportEvent& event);

  void on_pan_left();
  void on_pan_right();
  void on_pan_up();
  void on_pan_down();

  void on_increase_zoom();
  void on_decrease_zoom();
  void on_reset_zoom();

  void on_reset_font_size();
  void on_increase_font_size();
  void on_decrease_font_size();

  void on_load_tileset(const LoadTilesetEvent& event);
  void on_remove_tileset(const RemoveTilesetEvent& event);

  void on_select_tileset(const SelectTilesetEvent& event);

  void on_set_tileset_selection(const SetTilesetSelectionEvent& event);

  void on_rename_tileset(const RenameTilesetEvent& event);

  void on_add_row();
  void on_add_column();
  void on_remove_row();
  void on_remove_column();

  void on_resize_map(const ResizeMapEvent& event);

  void on_fix_tiles_in_map();

  void on_open_resize_map_dialog();

  void on_set_tile_format_encoding(const SetTileFormatEncodingEvent& event);
  void on_set_tile_format_compression(const SetTileFormatCompressionEvent& event);
  void on_set_zlib_compression_level(const SetZlibCompressionLevelEvent& event);
  void on_set_zstd_compression_level(const SetZstdCompressionLevelEvent& event);

  void on_add_layer(const AddLayerEvent& event);
  void on_remove_layer(const RemoveLayerEvent& event);
  void on_select_layer(const SelectLayerEvent& event);
  void on_move_layer_up(const MoveLayerUpEvent& event);
  void on_move_layer_down(const MoveLayerDownEvent& event);
  void on_duplicate_layer(const DuplicateLayerEvent& event);
  void on_set_layer_opacity(const SetLayerOpacityEvent& event);
  void on_set_layer_visible(const SetLayerVisibleEvent& event);
  void on_rename_layer(const RenameLayerEvent& event);

  void on_open_rename_layer_dialog(const OpenRenameLayerDialogEvent& event);

  void on_select_object(const SelectObjectEvent& event);
  void on_remove_object(const RemoveObjectEvent& event);
  void on_set_object_name(const SetObjectNameEvent& event);
  void on_move_object(const MoveObjectEvent& event);
  void on_set_object_visible(const SetObjectVisibleEvent& event);
  void on_set_object_tag(const SetObjectTagEvent& event);

  void on_spawn_object_context_menu(const SpawnObjectContextMenuEvent& event);

  void on_show_add_property_dialog();
  void on_show_rename_property_dialog(const ShowRenamePropertyDialogEvent& event);
  void on_show_change_property_type_dialog(
      const ShowChangePropertyTypeDialogEvent& event);

  void on_add_property(const AddPropertyEvent& event);
  void on_remove_property(const RemovePropertyEvent& event);
  void on_rename_property(const RenamePropertyEvent& event);
  void on_update_property(const UpdatePropertyEvent& event);
  void on_change_property_type(const ChangePropertyTypeEvent& event);

  void on_inspect_context(const InspectContextEvent& event);

  void on_open_component_editor();

  void on_define_component(const DefineComponentEvent& event);
  void on_undef_component(const UndefComponentEvent& event);
  void on_rename_component(const RenameComponentEvent& event);
  void on_add_component_attr(const AddComponentAttrEvent& event);
  void on_remove_component_attr(const RemoveComponentAttrEvent& event);
  void on_rename_component_attr(const RenameComponentAttrEvent& event);
  void on_duplicate_component_attr(const DuplicateComponentAttrEvent& event);
  void on_set_component_attr_type(const SetComponentAttrTypeEvent& event);
  void on_update_component(const UpdateComponentEvent& event);
  void on_attach_component(const AttachComponentEvent& event);
  void on_detach_component(const DetachComponentEvent& event);
  void on_update_attached_component(const UpdateAttachedComponentEvent& event);
  void on_reset_attached_component(const ResetAttachedComponentEvent& event);

  void on_toggle_ui();

  void on_reload_fonts();

  void on_quit();
};

}  // namespace tactile
