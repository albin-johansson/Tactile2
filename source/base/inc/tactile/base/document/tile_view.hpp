// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/container/pair.hpp"
#include "tactile/base/id.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/util/chrono.hpp"

namespace tactile {

class IDocumentVisitor;
class ITilesetView;
class IMetaView;

/**
 * A read-only view of a tile in a tileset.
 */
class ITileView
{
 public:
  TACTILE_INTERFACE_CLASS(ITileView);

  /**
   * Inspects the tile.
   *
   * \param visitor The visitor to use.
   */
  virtual void accept(IDocumentVisitor& visitor) const = 0;

  /**
   * Returns the tileset that hosts the tile.
   *
   * \return
   * A tileset view.
   */
  [[nodiscard]]
  virtual auto get_parent_tileset() const -> const ITilesetView& = 0;

  /**
   * Returns the local identifier associated with the tile.
   *
   * \return
   * A tile index.
   */
  [[nodiscard]]
  virtual auto get_index() const -> TileIndex = 0;

  /**
   * Returns the number of objects stored in the tile.
   *
   * \return
   * An object count.
   */
  [[nodiscard]]
  virtual auto object_count() const -> usize = 0;

  /**
   * Returns the number of frames in the associated animation, if any.
   *
   * \return
   * An animation frame count.
   */
  [[nodiscard]]
  virtual auto animation_frame_count() const -> usize = 0;

  /**
   * Returns the frame at a given index in the associated animation.
   *
   * \param index The animation frame index.
   *
   * \return
   * A tile index and frame duration pair.
   */
  [[nodiscard]]
  virtual auto get_animation_frame(usize index) const
      -> Pair<TileIndex, Milliseconds> = 0;

  /**
   * Returns a view of the associated metadata.
   *
   * \return
   * A metadata view.
   */
  [[nodiscard]]
  virtual auto get_meta() const -> const IMetaView& = 0;
};

}  // namespace tactile
