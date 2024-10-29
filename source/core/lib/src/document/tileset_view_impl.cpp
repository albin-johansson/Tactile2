// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/tileset_view_impl.hpp"

#include <algorithm>  // count_if

#include "tactile/base/document/document_visitor.hpp"
#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/document/tile_view_impl.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/io/texture.hpp"
#include "tactile/core/tile/tile.hpp"
#include "tactile/core/tile/tileset_types.hpp"

namespace tactile::core {

TilesetViewImpl::TilesetViewImpl(const MapDocument* document, const EntityID tileset_id)
  : mDocument {require_not_null(document, "null document")},
    mTilesetId {tileset_id},
    mMeta {mDocument, mTilesetId}
{}

auto TilesetViewImpl::accept(IDocumentVisitor& visitor) const -> std::expected<void, ErrorCode>
{
  if (const auto tileset_result = visitor.visit(*this); !tileset_result.has_value()) {
    return std::unexpected {tileset_result.error()};
  }

  const auto& registry = mDocument->get_registry();
  const auto& tileset = registry.get<CTileset>(mTilesetId);

  for (const auto tile_id : tileset.tiles) {
    if (is_tile_plain(registry, tile_id)) {
      continue;
    }

    const TileViewImpl tile_view {mDocument, this, tile_id};
    if (const auto tile_result = tile_view.accept(visitor); !tile_result.has_value()) {
      return std::unexpected {tile_result.error()};
    }
  }

  return {};
}

auto TilesetViewImpl::get_first_tile_id() const -> TileID
{
  const auto& registry = mDocument->get_registry();
  const auto& instance = registry.get<CTilesetInstance>(mTilesetId);

  return instance.tile_range.first_id;
}

auto TilesetViewImpl::tile_count() const -> std::size_t
{
  const auto& registry = mDocument->get_registry();
  const auto& tileset = registry.get<CTileset>(mTilesetId);

  return tileset.tiles.size();
}

auto TilesetViewImpl::tile_definition_count() const -> std::size_t
{
  const auto& registry = mDocument->get_registry();
  const auto& tileset = registry.get<CTileset>(mTilesetId);

  return saturate_cast<std::size_t>(std::ranges::count_if(
      tileset.tiles,
      [&registry](const EntityID tile_id) { return !is_tile_plain(registry, tile_id); }));
}

auto TilesetViewImpl::column_count() const -> std::size_t
{
  const auto& registry = mDocument->get_registry();

  const auto& tileset = registry.get<CTileset>(mTilesetId);
  const auto& texture = registry.get<CTexture>(mTilesetId);

  return saturate_cast<std::size_t>(texture.size.x() / tileset.tile_size.x());
}

auto TilesetViewImpl::get_tile_size() const -> Int2
{
  const auto& registry = mDocument->get_registry();
  const auto& tileset = registry.get<CTileset>(mTilesetId);

  return tileset.tile_size;
}

auto TilesetViewImpl::get_image_size() const -> Int2
{
  const auto& registry = mDocument->get_registry();
  const auto& texture = registry.get<CTexture>(mTilesetId);

  return texture.size;
}

auto TilesetViewImpl::get_image_path() const -> const std::filesystem::path&
{
  const auto& registry = mDocument->get_registry();
  const auto& texture = registry.get<CTexture>(mTilesetId);

  return texture.path;
}

auto TilesetViewImpl::get_meta() const -> const IMetaView&
{
  return mMeta;
}

auto TilesetViewImpl::get_filename() const -> std::string
{
  return get_image_path().stem().string();
}

}  // namespace tactile::core
