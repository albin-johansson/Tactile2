// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/container/expected.hpp"
#include "tactile/base/container/vector.hpp"
#include "tactile/base/id.hpp"
#include "tactile/base/int.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/util/chrono.hpp"
#include "tactile/core/entity/entity.hpp"

namespace tactile {

class Registry;

/// \addtogroup animation
/// \{

/**
 * Represents a single frame in a tile animation.
 */
struct AnimationFrame final
{
  /** The local identifier of the tile shown during the frame. */
  TileIndex tile_index;

  /** The amount of time the frame is active. */
  Milliseconds duration;
};

/**
 * A component that represents a sequential tile animation.
 */
struct CAnimation final
{
  /** The time of the last frame update. */
  SteadyClockInstant last_update;

  /** The index of the currently displayed frame. */
  usize frame_index;

  /** The sequence of frames the animation cycles through. */
  Vector<AnimationFrame> frames;
};

/**
 * Updates the state of all animations in a registry.
 *
 * \param registry The associated registry.
 */
void update_animations(Registry& registry);

/**
 * Adds an animation frame to a given tile.
 *
 * \details
 * If the tile isn't already animated, a \c CAnimation component will
 * automatically be added to the tile. Furthermore, a frame index equal to the
 * current number of frames in the animation is interpreted as a request
 * to add the frame to the end of the animation.
 *
 * \note
 * If successful, the animation progress will be reset.
 *
 * \pre The new frame must feature a valid tile index.
 *
 * \param registry    The associated registry.
 * \param tile_entity The target tile entity.
 * \param frame_index The index of the new frame.
 * \param frame       The new frame.
 *
 * \return
 * Nothing if the frame was added successfully; an error code otherwise.
 */
auto add_animation_frame(Registry& registry,
                         EntityID tile_entity,
                         usize frame_index,
                         const AnimationFrame& frame) -> Result<void>;

/**
 * Removes an animation frame from a given tile.
 *
 * \details
 * The animation will be removed from the tile if the specified frame is the
 * last one in the animation.
 *
 * \note
 * If successful, the animation progress will be reset.
 *
 * \pre The specified tile must be animated.
 *
 * \param registry    The associated registry.
 * \param tile_entity The target tile entity.
 * \param frame_index The index of the frame to remove.
 *
 * \return
 * Nothing if the frame was removed successfully; an error code otherwise.
 */
auto remove_animation_frame(Registry& registry,
                            EntityID tile_entity,
                            usize frame_index) -> Result<void>;

/// \}

}  // namespace tactile
