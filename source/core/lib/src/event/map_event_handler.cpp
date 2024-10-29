// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/map_event_handler.hpp"

#include <utility>  // move

#include <magic_enum.hpp>

#include "tactile/base/io/save/save_format.hpp"
#include "tactile/base/numeric/vec_format.hpp"
#include "tactile/base/runtime/runtime.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/document/map_view_impl.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/platform/file_dialog.hpp"
#include "tactile/core/ui/widget_manager.hpp"

namespace tactile::core {

MapEventHandler::MapEventHandler(Model* model,
                                 ui::WidgetManager* widget_manager,
                                 IRuntime* runtime)
  : mModel {require_not_null(model, "null model")},
    mWidgetManager {require_not_null(widget_manager, "null widget manager")},
    mRuntime {require_not_null(runtime, "null runtime")}
{}

void MapEventHandler::install(EventDispatcher& dispatcher)
{
  TACTILE_CORE_DEBUG("Installing map event handler");

  using Self = MapEventHandler;
  dispatcher.bind<ShowNewMapDialogEvent, &Self::on_show_new_map_dialog>(this);
  dispatcher.bind<ShowOpenMapDialogEvent, &Self::on_show_open_map_dialog>(this);
  dispatcher.bind<ShowGodotExportDialogEvent, &Self::on_show_godot_export_dialog>(this);
  dispatcher.bind<CreateMapEvent, &Self::on_create_map>(this);
  dispatcher.bind<ExportAsGodotSceneEvent, &Self::on_export_as_godot_scene>(this);
  // TODO ResizeMapEvent
  // TODO FixTilesInMapEvent
  // TODO InspectMapEvent? (or InspectContextEvent?)
  // TODO SetTileEncodingEvent?
  // TODO SetTileCompressionEvent?
  // TODO SetCompressionLevelEvent?
}

void MapEventHandler::on_show_new_map_dialog(const ShowNewMapDialogEvent&)
{
  TACTILE_CORE_TRACE("ShowNewMapDialogEvent");
  mWidgetManager->get_new_map_dialog().open();
}

void MapEventHandler::on_show_open_map_dialog(const ShowOpenMapDialogEvent&)
{
  TACTILE_CORE_TRACE("ShowOpenMapDialogEvent");

  const auto map_path = FileDialog::open_map();
  if (!map_path.has_value()) {
    return;
  }

  TACTILE_CORE_TRACE("Trying to load map {}", map_path->string());
  const auto format_id = _guess_save_format(*map_path);
  if (!format_id.has_value()) {
    TACTILE_CORE_ERROR("Unknown save format for extension '{}'",
                      map_path->extension().string());
    return;
  }

  const auto* save_format = mRuntime->get_save_format(*format_id);
  if (!save_format) {
    TACTILE_CORE_ERROR("Found no suitable installed save format");
    return;
  }

  // TODO
  const SaveFormatReadOptions read_options {
    .base_dir = map_path->parent_path(),
    .strict_mode = false,
  };

  const auto ir_map = save_format->load_map(*map_path, read_options);
  if (!ir_map.has_value()) {
    TACTILE_CORE_ERROR("Could not load map: {}", to_string(ir_map.error()));
    return;
  }

  auto& document_manager = mModel->get_document_manager();

  const auto document_uuid =
      document_manager.create_and_open_map(*mRuntime->get_renderer(), *ir_map);
  if (!document_uuid.has_value()) {
    TACTILE_CORE_ERROR("Could not create map document: {}", to_string(document_uuid.error()));
    return;
  }

  auto& document = document_manager.get_document(*document_uuid);
  document.set_format(*format_id);
}

void MapEventHandler::on_show_godot_export_dialog(const ShowGodotExportDialogEvent&)
{
  TACTILE_CORE_TRACE("ShowGodotExportDialogEvent");
  mWidgetManager->get_godot_export_dialog().open();
}

void MapEventHandler::on_create_map(const CreateMapEvent& event)
{
  TACTILE_CORE_TRACE("CreateMapEvent(orientation: {}, size: {}, tile_size: {})",
                    magic_enum::enum_name(event.spec.orientation),
                    event.spec.extent,
                    event.spec.tile_size);

  auto& document_manager = mModel->get_document_manager();
  const auto document_uuid = document_manager.create_and_open_map(event.spec);
  if (document_uuid.has_value()) {
    TACTILE_CORE_DEBUG("Created map document (uuid: {})", *document_uuid);
  }
}

void MapEventHandler::on_export_as_godot_scene(const ExportAsGodotSceneEvent& event) const
{
  TACTILE_CORE_TRACE("ExportMapEvent");

  const auto* save_format = mRuntime->get_save_format(SaveFormatId::kGodotTscn);
  if (!save_format) {
    TACTILE_CORE_ERROR("Godot plugin is not enabled");
    return;
  }

  const auto* document = dynamic_cast<const MapDocument*>(mModel->get_current_document());
  if (!document) {
    TACTILE_CORE_ERROR("No current document");
    return;
  }

  SaveFormatExtraSettings extra_settings {};
  extra_settings["version"] = Attribute {event.version};
  extra_settings["ellipse_polygon_vertices"] = Attribute {32};

  const SaveFormatWriteOptions options {
    .extra = std::move(extra_settings),
    .base_dir = event.project_dir,
    .use_external_tilesets = false,
    .use_indentation = false,
    .fold_tile_layer_data = false,
  };

  const MapViewImpl map_view {document};
  const auto save_result = save_format->save_map(map_view, options);

  if (!save_result.has_value()) {
    TACTILE_CORE_ERROR("Could not export Godot scene: {}", to_string(save_result.error()));
  }
}

auto MapEventHandler::_guess_save_format(const std::filesystem::path& path)
    -> std::optional<SaveFormatId>
{
  const auto extension = path.extension();

  if (extension == ".yaml" || extension == ".yml") {
    return SaveFormatId::kTactileYaml;
  }

  if (extension == ".tmj" || extension == ".json") {
    return SaveFormatId::kTiledTmj;
  }

  if (extension == ".tmx" || extension == ".xml") {
    return SaveFormatId::kTiledTmx;
  }

  return std::nullopt;
}

}  // namespace tactile::core
