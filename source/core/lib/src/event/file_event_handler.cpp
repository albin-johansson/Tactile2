// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/file_event_handler.hpp"

#include "tactile/base/io/save/save_format.hpp"
#include "tactile/base/runtime/runtime.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/document/map_view_impl.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"

namespace tactile::core {

FileEventHandler::FileEventHandler(Model* model, IRuntime* runtime)
  : mModel {require_not_null(model, "null model")},
    mRuntime {require_not_null(runtime, "null runtime")}
{}

void FileEventHandler::install(EventDispatcher& dispatcher)
{
  using Self = FileEventHandler;

  // clang-format off
  dispatcher.bind<SaveEvent, &Self::on_save>(this);
  dispatcher.bind<SaveAsEvent, &Self::on_save_as>(this);
  dispatcher.bind<ReopenLastClosedFileEvent, &Self::on_reopen_last_closed_file>(this);
  dispatcher.bind<ClearFileHistoryEvent, &Self::on_clear_file_history>(this);
  dispatcher.bind<CloseEvent, &Self::on_close>(this);
  dispatcher.bind<QuitEvent, &Self::on_quit>(this);
  // clang-format on
}

void FileEventHandler::on_save(const SaveEvent& event)
{
  TACTILE_CORE_TRACE("SaveEvent");

  const auto* document = dynamic_cast<const MapDocument*>(mModel->get_current_document());
  if (!document) {
    TACTILE_CORE_ERROR("No map document is active");
    return;
  }

  const auto* document_path = document->get_path();
  if (!document_path) {
    TACTILE_CORE_ERROR("Document has no associated path");
    return;
  }

  const auto* save_format = mRuntime->get_save_format(document->get_format());
  if (!save_format) {
    TACTILE_CORE_ERROR("Could not find suitable save format");
    return;
  }

  const MapViewImpl map_view {document};

  // TODO
  const SaveFormatWriteOptions options {
    .base_dir = document_path->parent_path(),
    .use_external_tilesets = false,
    .use_indentation = true,
    .fold_tile_layer_data = false,
  };

  const auto save_result = save_format->save_map(map_view, options);
  if (!save_result.has_value()) {
    TACTILE_CORE_ERROR("Could not save map: {}", to_string(save_result.error()));
  }
}

void FileEventHandler::on_save_as(const SaveAsEvent& event)
{
  TACTILE_CORE_TRACE("SaveAsEvent(path: {})", event.path.string());

  auto* document = dynamic_cast<MapDocument*>(mModel->get_current_document());
  if (!document) {
    TACTILE_CORE_ERROR("No map document is active");
    return;
  }

  document->set_path(event.path);
  on_save(SaveEvent {});
}

void FileEventHandler::on_reopen_last_closed_file(const ReopenLastClosedFileEvent& event)
{
  TACTILE_CORE_TRACE("ReopenLastClosedFileEvent");
  // TODO
}

void FileEventHandler::on_clear_file_history(const ClearFileHistoryEvent& event)
{
  TACTILE_CORE_TRACE("ClearFileHistoryEvent");
  // TODO
}

void FileEventHandler::on_close(const CloseEvent& event)
{
  TACTILE_CORE_TRACE("CloseEvent");
  // TODO
}

void FileEventHandler::on_quit(const QuitEvent& event)
{
  TACTILE_CORE_TRACE("QuitEvent");
  // TODO
}

// TODO OpenDocumentEvent?
// TODO SelectDocumentEvent
// TODO CloseDocumentEvent

}  // namespace tactile::core
