#pragma once

#include <filesystem>  // path
#include <string>      // string

#include <tactile-base/tactile_std.hpp>

#include "core/region.hpp"

namespace Tactile {

/// \addtogroup events
/// \{

struct AddTilesetEvent final
{
  std::filesystem::path path;
  int tile_width{};
  int tile_height{};
};

struct RemoveTilesetEvent final
{
  TilesetID id;
};

struct SelectTilesetEvent final
{
  TilesetID id;
};

struct SetTilesetSelectionEvent final
{
  Region selection;
};

struct ShowTilesetPropertiesEvent final
{
  TilesetID id;
};

/// \brief Emitted when the name of a tileset should be changed.
struct SetTilesetNameEvent final
{
  TilesetID id{};    ///< ID of target tileset.
  std::string name;  ///< The new tileset name.
};

/// \} End of group events

}  // namespace Tactile
