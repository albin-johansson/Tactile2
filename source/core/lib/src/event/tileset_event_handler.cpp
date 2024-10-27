// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/tileset_event_handler.hpp"

#include "tactile/base/numeric/vec_format.hpp"
#include "tactile/base/render/renderer.hpp"
#include "tactile/base/render/texture.hpp"
#include "tactile/core/cmd/tile/add_tileset_command.hpp"
#include "tactile/core/debug/validation.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/ui/widget_manager.hpp"

namespace tactile::core {

TilesetEventHandler::TilesetEventHandler(Model* model,
                                         IRenderer* renderer,
                                         ui::WidgetManager* widget_manager)
  : mModel {require_not_null(model, "null model")},
    mRenderer {require_not_null(renderer, "null renderer")},
    mWidgetManager {require_not_null(widget_manager, "null widget manager")}
{}

void TilesetEventHandler::install(EventDispatcher& dispatcher)
{
  using Self = TilesetEventHandler;

  dispatcher.bind<ShowNewTilesetDialogEvent, &Self::on_show_new_tileset_dialog>(this);
  dispatcher.bind<AddTilesetEvent, &Self::on_add_tileset>(this);

  // TODO RemoveTilesetEvent
  // TODO RenameTilesetEvent
  // TODO SetTilesetSelectionEvent
  // TODO SetTileAnimationFrameDurationEvent
  // TODO EnableAnimationFrameSelectionMode
  // TODO AddTileAnimationFrameEvent
  // TODO RemoveTileAnimationFrameEvent
  // TODO MoveAnimationFrameForwardsEvent
  // TODO MoveAnimationFrameBackwardsEvent
  // TODO DeleteTileAnimationEvent
  // TODO RenameTileEvent
}

void TilesetEventHandler::on_show_new_tileset_dialog(const ShowNewTilesetDialogEvent&)
{
  TACTILE_CORE_TRACE("ShowNewTilesetDialogEvent");
  mWidgetManager->get_new_tileset_dialog().open();
}

void TilesetEventHandler::on_add_tileset(const AddTilesetEvent& event)
{
  TACTILE_CORE_TRACE("AddTilesetEvent(path: {}, tile size: {})",
                    event.texture_path.string(),
                    event.tile_size);

  const auto texture_id = mRenderer->load_texture(event.texture_path);
  if (!texture_id.has_value()) {
    TACTILE_CORE_ERROR("Could not load tileset texture: {}", to_string(texture_id.error()));
    return;
  }

  const auto* texture = mRenderer->find_texture(*texture_id);
  if (!texture) {
    TACTILE_CORE_ERROR("Could not find tileset texture");
    return;
  }

  const auto texture_size = texture->get_size();

  mModel->push_map_command<AddTilesetCommand>(TilesetSpec {
    .tile_size = event.tile_size,
    .texture =
        CTexture {
          .raw_handle = texture->get_handle(),
          .id = *texture_id,
          .size = Int2 {texture_size.width, texture_size.height},
          .path = texture->get_path(),
        },
  });
}

}  // namespace tactile::core
