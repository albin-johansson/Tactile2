#include "parse_tile_layer.hpp"

#include "core/map/layers/tile_layer.hpp"
#include "parse_tile_data.hpp"
#include "xml_utils.hpp"

namespace Tactile::IO {

auto ParseTileLayer(const pugi::xml_node node, LayerData& layer) -> ParseError
{
  auto& data = layer.data.emplace<TileLayerData>();

  if (const auto width = GetInt(node, "width"))
  {
    data.col_count = col_t{*width};
  }
  else
  {
    return ParseError::LayerMissingWidth;
  }

  if (const auto height = GetInt(node, "height"))
  {
    data.row_count = row_t{*height};
  }
  else
  {
    return ParseError::LayerMissingHeight;
  }

  data.tiles = MakeTileMatrix(data.row_count, data.col_count);
  if (const auto err = ParseTileData(node, data.col_count, data.tiles);
      err != ParseError::None)
  {
    return err;
  }

  return ParseError::None;
}

}  // namespace Tactile::IO
