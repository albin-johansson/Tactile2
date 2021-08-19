#include "to_map_document.hpp"

#include <cassert>  // assert
#include <utility>  // move
#include <variant>  // get

#include "io/parsing/parse_ir.hpp"
#include "utils/load_texture.hpp"

namespace Tactile::IO {
namespace {

void AddProperties(IPropertyContext& context,
                   const std::vector<PropertyData>& properties)
{
  for (const auto& data : properties)
  {
    context.AddProperty(data.name, data.property);
  }
}

[[nodiscard]] auto MakeTileset(const TilesetData& data) -> Shared<Tileset>
{
  const auto info = LoadTexture(data.absolute_image_path);

  auto tileset = std::make_shared<Tileset>(data.first_id,
                                           info.value(),
                                           data.tile_width,
                                           data.tile_height);
  tileset->SetName(data.name);

  for (const auto& tile : data.tiles)
  {
    if (!tile.animation.empty())
    {
      TileAnimation animation;
      for (const auto& frame : tile.animation)
      {
        animation.AddFrame({.tile = data.first_id + frame.tile,
                            .duration = cen::milliseconds<uint32>{frame.duration}});
      }

      tileset->SetAnimation(data.first_id + tile.id, std::move(animation));
    }
  }

  AddProperties(*tileset, data.properties);

  return tileset;
}

[[nodiscard]] auto MakeTileLayer(const TileLayerData& data) -> SharedLayer
{
  auto layer = std::make_shared<TileLayer>(data.row_count, data.col_count);

  for (auto row = 0_row; row < data.row_count; ++row)
  {
    for (auto col = 0_col; col < data.col_count; ++col)
    {
      const MapPosition pos{row, col};
      const auto tile = data.tiles.at(pos.GetRowIndex()).at(pos.GetColumnIndex());
      layer->SetTile(pos, tile);
    }
  }

  return layer;
}

[[nodiscard]] auto MakeObject(const ObjectData& data) -> Object
{
  Object object{data.type};

  object.SetName(data.name);
  object.SetX(data.x);
  object.SetY(data.y);
  object.SetWidth(data.width);
  object.SetHeight(data.height);
  object.SetVisible(data.visible);

  if (!data.custom_type.empty())
  {
    object.SetCustomType(data.custom_type);
  }

  AddProperties(object, data.properties);

  return object;
}

[[nodiscard]] auto MakeObjectLayer(const ObjectLayerData& data) -> SharedLayer
{
  auto layer = std::make_shared<ObjectLayer>();

  for (const auto& objectData : data.objects)
  {
    layer->AddObject(objectData.id, MakeObject(objectData));
  }

  return layer;
}

[[nodiscard]] auto MakeLayer(const LayerData& data) -> SharedLayer;

[[nodiscard]] auto MakeGroupLayer(const GroupLayerData& data) -> SharedLayer
{
  auto layer = std::make_shared<GroupLayer>();

  for (const auto& layerData : data.layers)
  {
    layer->AddLayer(layerData->id, MakeLayer(*layerData));
  }

  return layer;
}

[[nodiscard]] auto MakeLayer(const LayerData& data) -> SharedLayer
{
  SharedLayer layer;

  if (data.type == LayerType::TileLayer)
  {
    layer = MakeTileLayer(std::get<TileLayerData>(data.data));
  }
  else if (data.type == LayerType::ObjectLayer)
  {
    layer = MakeObjectLayer(std::get<ObjectLayerData>(data.data));
  }
  else if (data.type == LayerType::GroupLayer)
  {
    layer = MakeGroupLayer(std::get<GroupLayerData>(data.data));
  }

  assert(layer);
  layer->SetName(data.name);
  layer->SetOpacity(data.opacity);
  layer->SetVisible(data.is_visible);

  AddProperties(*layer, data.properties);

  return layer;
}

}  // namespace

auto ToMapDocument(const MapData& data) -> Unique<MapDocument>
{
  auto document = std::make_unique<MapDocument>();
  document->SetNextLayerId(data.next_layer_id);
  document->SetNextObjectId(data.next_object_id);
  document->SetPath(data.absolute_path);

  auto& map = document->GetMap();
  map.SetTileWidth(data.tile_width);
  map.SetTileHeight(data.tile_height);

  auto& tilesets = document->GetTilesets();
  for (const auto& tilesetData : data.tilesets)
  {
    const auto id [[maybe_unused]] = tilesets.Add(MakeTileset(tilesetData));
  }

  for (const auto& layerData : data.layers)
  {
    document->AddLayer(layerData.id, MakeLayer(layerData));
  }

  AddProperties(*document, data.properties);

  if (!data.layers.empty())
  {
    document->SelectLayer(data.layers.front().id);
  }

  document->ResetHistory();  // Make sure there is no undo/redo history

  return document;
}

}  // namespace Tactile::IO
