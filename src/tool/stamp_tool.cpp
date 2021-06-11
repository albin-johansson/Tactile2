#include "stamp_tool.hpp"

#include "model.hpp"
#include "tile_layer.hpp"

namespace tactile {

using namespace core;

StampTool::StampTool(Model* model) : AMouseTool{model}
{
  mOldState.reserve(32);
  mSequence.reserve(32);
}

void StampTool::update_stamp_sequence(MapDocument& map,
                                      const Tileset& ts,
                                      const Position& origin)
{
  const auto callable = [&](const Position& mapPos,
                            const Position& tilesetPos) {
    if (map.InBounds(mapPos))
    {
      const auto newID = ts.TileAt(tilesetPos);

      auto* tileLayer = map.GetTileLayer(map.CurrentLayerId().value());
      Q_ASSERT(tileLayer);

      if (!mOldState.contains(mapPos))
      {
        mOldState.emplace(mapPos, tileLayer->TileAt(mapPos).value());
      }
      mSequence.emplace(mapPos, newID);

      tileLayer->SetTile(mapPos, newID);
    }
  };

  ts.VisitSelection(origin, callable);
}

void StampTool::OnPressed(QMouseEvent* event, const QPointF& mapPosition)
{
  if (auto* document = GetModel()->CurrentDocument())
  {
    auto* tileset = document->CurrentTileset();
    if (!tileset || !tileset->GetSelection().has_value())
    {
      return;
    }

    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
      const auto pos = TranslateMousePosition(event->pos(), mapPosition);
      if (pos)
      {
        update_stamp_sequence(*document, *tileset, *pos);
        emit GetModel()->S_Redraw();
      }
    }
  }
}

void StampTool::OnMoved(QMouseEvent* event, const QPointF& mapPosition)
{
  if (auto* document = GetModel()->CurrentDocument())
  {
    const auto* tileset = document->CurrentTileset();
    if (!tileset || !tileset->GetSelection())
    {
      return;
    }

    if (const auto pos = TranslateMousePosition(event->pos(), mapPosition))
    {
      emit GetModel()->S_EnableStampPreview(*pos);

      if (event->buttons() & Qt::MouseButton::LeftButton)
      {
        update_stamp_sequence(*document, *tileset, *pos);
      }

      emit GetModel()->S_Redraw();
    }
    else
    {
      // Mouse is outside of map, so disable preview
      emit GetModel()->S_DisableStampPreview();
    }
  }
}

void StampTool::OnReleased(QMouseEvent* event, const QPointF&)
{
  if (auto* document = GetModel()->CurrentDocument())
  {
    auto* tileset = document->CurrentTileset();
    if (!tileset || !tileset->GetSelection())
    {
      return;
    }

    if (event->button() == Qt::MouseButton::LeftButton)
    {
      document->AddStampSequence(std::move(mOldState), std::move(mSequence));

      // Clearing the maps allows for them to be used after being moved from
      mOldState.clear();
      mSequence.clear();
    }
  }
}

void StampTool::OnExited(QEvent*)
{
  emit GetModel()->S_DisableStampPreview();
}

void StampTool::Disable()
{
  emit GetModel()->S_DisableStampPreview();
}

}  // namespace tactile
