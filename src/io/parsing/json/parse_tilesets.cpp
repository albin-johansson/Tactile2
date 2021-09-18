#include "parse_tilesets.hpp"

#include <cassert>     // assert
#include <filesystem>  // exists, weakly_canonical
#include <string>      // string

#include "parse_fancy_tiles.hpp"
#include "parse_properties.hpp"
#include "read_json.hpp"

namespace Tactile::IO {
namespace {

[[nodiscard]] auto ParseFirstTileId(const JSON& json, TileID& id) -> ParseError
{
  if (const auto it = json.find("firstgid"); it != json.end()) {
    id = TileID{it->get<TileID::value_type>()};
    return ParseError::None;
  }
  else {
    return ParseError::TilesetMissingFirstGid;
  }
}

[[nodiscard]] auto ParseTilesetCommon(const JSON& json,
                                      TilesetData& data,
                                      const std::filesystem::path& directory)
    -> ParseError
{
  if (const auto it = json.find("tilewidth"); it != json.end()) {
    it->get_to(data.tile_width);
  }
  else {
    return ParseError::TilesetMissingTileWidth;
  }

  if (const auto it = json.find("tileheight"); it != json.end()) {
    it->get_to(data.tile_height);
  }
  else {
    return ParseError::TilesetMissingTileHeight;
  }

  const auto relativeImagePath = json.find("image");
  if (relativeImagePath == json.end()) {
    return ParseError::TilesetMissingImagePath;
  }

  const auto absoluteImagePath = std::filesystem::weakly_canonical(
      directory / relativeImagePath->get<std::string>());

  if (std::filesystem::exists(absoluteImagePath)) {
    data.absolute_image_path = absoluteImagePath;
  }
  else {
    return ParseError::TilesetImageDoesNotExist;
  }

  if (const auto it = json.find("name"); it != json.end()) {
    it->get_to(data.name);
  }
  else {
    return ParseError::TilesetMissingName;
  }

  if (const auto err = ParseFancyTiles(json, data); err != ParseError::None) {
    return err;
  }

  if (const auto err = ParseProperties(json, data.properties); err != ParseError::None) {
    return err;
  }

  return ParseError::None;
}

[[nodiscard]] auto ParseExternalTileset(const JSON& json,
                                        TilesetData& data,
                                        const std::filesystem::path& directory)
    -> ParseError
{
  assert(json.contains("source"));

  const auto source = json.at("source").get<std::string>();
  const auto path = std::filesystem::weakly_canonical(directory / source);

  try {
    const auto external = ReadJson(path);
    return ParseTilesetCommon(external, data, directory);
  }
  catch (...) {
    return ParseError::CouldNotReadExternalTileset;
  }
}

[[nodiscard]] auto ParseTileset(const JSON& json,
                                TilesetData& data,
                                const std::filesystem::path& directory) -> ParseError
{
  if (const auto err = ParseFirstTileId(json, data.first_id); err != ParseError::None) {
    return err;
  }

  if (json.contains("source")) {
    return ParseExternalTileset(json, data, directory);
  }
  else {
    return ParseTilesetCommon(json, data, directory);
  }

  return ParseError::None;
}

}  // namespace

auto ParseTilesets(const JSON& json,
                   std::vector<TilesetData>& tilesets,
                   const std::filesystem::path& directory) -> ParseError
{
  const auto it = json.find("tilesets");
  if (it == json.end()) {
    return ParseError::MapMissingTilesets;
  }

  for (const auto& [key, value] : it->items()) {
    auto& data = tilesets.emplace_back();
    if (const auto err = ParseTileset(value, data, directory); err != ParseError::None) {
      return err;
    }
  }

  return ParseError::None;
}

}  // namespace Tactile::IO
