// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/container/vector.hpp"
#include "tactile/base/document/tile_view.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/core/document/meta_view_impl.hpp"
#include "tactile/core/document/object_view_impl.hpp"
#include "tactile/core/entity/entity.hpp"

namespace tactile {

class IDocument;
class ITilesetView;

/**
 * A tile view implementation.
 */
class TileViewImpl final : public ITileView
{
 public:
  /**
   * Creates a tile view.
   *
   * \param document     The associated document.
   * \param tileset_view A view of the parent tileset.
   * \param tile_id      The tile entity identifier.
   */
  TileViewImpl(const IDocument* document,
               const ITilesetView* tileset_view,
               EntityID tile_id);

  void accept(IDocumentVisitor& visitor) const override;

  [[nodiscard]]
  auto get_parent_tileset() const -> const ITilesetView& override;

  [[nodiscard]]
  auto get_index() const -> TileIndex override;

  [[nodiscard]]
  auto object_count() const -> usize override;

  [[nodiscard]]
  auto animation_frame_count() const -> usize override;

  [[nodiscard]]
  auto get_animation_frame(usize index) const
      -> Pair<TileIndex, Milliseconds> override;

  [[nodiscard]]
  auto get_meta() const -> const IMetaView& override;

 private:
  const IDocument* mDocument;
  const ITilesetView* mTilesetView;
  EntityID mTileId;
  MetaViewImpl mMeta;
};

}  // namespace tactile