#pragma once

#include <ostream>  // ostream

#include "aliases/czstring.hpp"

namespace Tactile::IO {

/// \addtogroup io
/// \{

/**
 * \brief Provides various map parsing error codes.
 *
 * \see `MapParser`
 * \see `GetCause(ParseError)`
 */
enum class ParseError
{
  None,
  Unknown,

  MapUnsupportedExtension,
  MapUnsupportedOrientation,
  MapUnsupportedInfinite,
  MapDoesNotExist,
  MapMissingNextLayerId,
  MapMissingNextObjectId,
  MapMissingTilesets,
  MapMissingLayers,
  MapMissingTileWidth,
  MapMissingTileHeight,

  LayerMissingWidth,
  LayerMissingHeight,
  LayerMissingType,
  LayerMissingId,
  LayerUnknownType,

  CouldNotParseTiles,
  UnsupportedTileEncoding,

  CouldNotReadExternalTileset,
  TilesetMissingFirstGid,
  TilesetMissingTileWidth,
  TilesetMissingTileHeight,
  TilesetMissingImagePath,
  TilesetMissingName,
  TilesetImageDoesNotExist,

  CouldNotParseProperty,
  PropertyMissingName,
  PropertyUnknownType,

  ObjectMissingId
};

/**
 * \brief Returns a potential cause of the specified parse error.
 *
 * \details The returned string is meant to help the user to identify the potential
 * cause of a parse error.
 *
 * \param error the error code to obtain a potential cause for.
 *
 * \return a short explanation of the potential cause of the error.
 */
[[nodiscard]] auto GetCause(ParseError error) -> czstring;

/**
 * \brief Prints a textual representation of a parse error enumerator.
 *
 * \param stream the output stream that will be used.
 * \param error the error code that will be printed.
 *
 * \return the used stream.
 */
auto operator<<(std::ostream& stream, ParseError error) -> std::ostream&;

/// \} End of group io

}  // namespace Tactile::IO
