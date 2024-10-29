// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/tile_view_impl.hpp"

#include "tactile/base/document/document.hpp"
#include "tactile/base/document/document_visitor.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/tile/animation_types.hpp"
#include "tactile/core/tile/tile_types.hpp"

namespace tactile::core {

TileViewImpl::TileViewImpl(const IDocument* document,
                           const ITilesetView* tileset_view,
                           const EntityID tile_id)
  : mDocument {require_not_null(document, "null document")},
    mTilesetView {require_not_null(tileset_view, "null tileset view")},
    mTileId {tile_id},
    mMeta {document, tile_id}
{}

auto TileViewImpl::accept(IDocumentVisitor& visitor) const -> std::expected<void, ErrorCode>
{
  if (const auto tile_result = visitor.visit(*this); !tile_result.has_value()) {
    return std::unexpected {tile_result.error()};
  }

  const auto& registry = mDocument->get_registry();
  const auto& tile = registry.get<CTile>(mTileId);

  for (const auto object_id : tile.objects) {
    const ObjectViewImpl object_view {mDocument, this, object_id};
    if (const auto object_result = object_view.accept(visitor); !object_result.has_value()) {
      return std::unexpected {object_result.error()};
    }
  }

  return {};
}

auto TileViewImpl::get_parent_tileset() const -> const ITilesetView&
{
  return *mTilesetView;
}

auto TileViewImpl::get_index() const -> TileIndex
{
  const auto& registry = mDocument->get_registry();
  const auto& tile = registry.get<CTile>(mTileId);

  return tile.index;
}

auto TileViewImpl::object_count() const -> std::size_t
{
  const auto& registry = mDocument->get_registry();
  const auto& tile = registry.get<CTile>(mTileId);

  return tile.objects.size();
}

auto TileViewImpl::animation_frame_count() const -> std::size_t
{
  const auto& registry = mDocument->get_registry();

  if (const auto* animation = registry.find<CAnimation>(mTileId)) {
    return animation->frames.size();
  }

  return 0;
}

auto TileViewImpl::get_animation_frame(const std::size_t index) const
    -> std::pair<TileIndex, std::chrono::milliseconds>
{
  const auto& registry = mDocument->get_registry();

  const auto& animation = registry.get<CAnimation>(mTileId);
  const auto& frame = animation.frames.at(index);

  return {frame.tile_index, frame.duration};
}

auto TileViewImpl::get_meta() const -> const IMetaView&
{
  return mMeta;
}

}  // namespace tactile::core
