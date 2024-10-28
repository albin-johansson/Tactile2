// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/tile/animation.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/tile/animation_types.hpp"
#include "tactile/core/tile/tile.hpp"

namespace tactile::core {
namespace {

// tactile::core::update_animations
TEST(Animation, UpdateAnimations)
{
  Registry registry {};

  auto add_frame = [&registry](EntityID tile_entity,
                               const std::size_t frame_index,
                               const TileIndex tile_index) {
    const AnimationFrame frame {tile_index, std::chrono::milliseconds::zero()};
    return add_animation_frame(registry, tile_entity, frame_index, frame);
  };

  const auto tile1_entity = make_tile(registry, TileIndex {1});
  const auto tile2_entity = make_tile(registry, TileIndex {2});
  const auto tile3_entity = make_tile(registry, TileIndex {3});

  EXPECT_TRUE(add_frame(tile1_entity, 0, TileIndex {10}).has_value());
  EXPECT_TRUE(add_frame(tile1_entity, 1, TileIndex {11}).has_value());
  EXPECT_TRUE(add_frame(tile1_entity, 2, TileIndex {12}).has_value());
  EXPECT_TRUE(add_frame(tile2_entity, 0, TileIndex {20}).has_value());
  EXPECT_TRUE(add_frame(tile2_entity, 1, TileIndex {21}).has_value());
  EXPECT_TRUE(add_frame(tile3_entity, 0, TileIndex {30}).has_value());

  EXPECT_EQ(registry.get<CAnimation>(tile1_entity).frame_index, 0);
  EXPECT_EQ(registry.get<CAnimation>(tile2_entity).frame_index, 0);
  EXPECT_EQ(registry.get<CAnimation>(tile3_entity).frame_index, 0);

  update_animations(registry);

  EXPECT_EQ(registry.get<CAnimation>(tile1_entity).frame_index, 1);
  EXPECT_EQ(registry.get<CAnimation>(tile2_entity).frame_index, 1);
  EXPECT_EQ(registry.get<CAnimation>(tile3_entity).frame_index, 0);

  update_animations(registry);

  EXPECT_EQ(registry.get<CAnimation>(tile1_entity).frame_index, 2);
  EXPECT_EQ(registry.get<CAnimation>(tile2_entity).frame_index, 0);
  EXPECT_EQ(registry.get<CAnimation>(tile3_entity).frame_index, 0);

  update_animations(registry);

  EXPECT_EQ(registry.get<CAnimation>(tile1_entity).frame_index, 0);
  EXPECT_EQ(registry.get<CAnimation>(tile2_entity).frame_index, 1);
  EXPECT_EQ(registry.get<CAnimation>(tile3_entity).frame_index, 0);
}

// tactile::core::add_animation_frame
TEST(Animation, AddAnimationFrame)
{
  Registry registry {};
  const auto tile_entity = make_tile(registry, TileIndex {100});

  constexpr AnimationFrame frame1 {TileIndex {110}, std::chrono::milliseconds {123}};
  constexpr AnimationFrame frame2 {TileIndex {120}, std::chrono::milliseconds {321}};
  constexpr AnimationFrame frame3 {TileIndex {130}, std::chrono::milliseconds {987}};
  constexpr AnimationFrame frame4 {TileIndex {140}, std::chrono::milliseconds {593}};
  constexpr AnimationFrame frame5 {TileIndex {150}, std::chrono::milliseconds {739}};

  ASSERT_FALSE(registry.has<CAnimation>(tile_entity));

  // Bad frame index
  EXPECT_FALSE(add_animation_frame(registry, tile_entity, 1, frame1).has_value());
  EXPECT_FALSE(registry.has<CAnimation>(tile_entity));

  // Create animation
  EXPECT_TRUE(add_animation_frame(registry, tile_entity, 0, frame1).has_value());
  EXPECT_TRUE(registry.has<CAnimation>(tile_entity));

  // Prepend
  EXPECT_TRUE(add_animation_frame(registry, tile_entity, 0, frame2).has_value());

  // Insert
  EXPECT_TRUE(add_animation_frame(registry, tile_entity, 1, frame3).has_value());

  // Append
  EXPECT_TRUE(add_animation_frame(registry, tile_entity, 3, frame4).has_value());

  // Bad index
  EXPECT_FALSE(add_animation_frame(registry, tile_entity, 5, frame5).has_value());

  const auto& animation = registry.get<CAnimation>(tile_entity);

  // [ frame2, frame3, frame1, frame4 ]
  ASSERT_EQ(animation.frames.size(), 4);

  EXPECT_EQ(animation.frames[0].tile_index, frame2.tile_index);
  EXPECT_EQ(animation.frames[0].duration, frame2.duration);

  EXPECT_EQ(animation.frames[1].tile_index, frame3.tile_index);
  EXPECT_EQ(animation.frames[1].duration, frame3.duration);

  EXPECT_EQ(animation.frames[2].tile_index, frame1.tile_index);
  EXPECT_EQ(animation.frames[2].duration, frame1.duration);

  EXPECT_EQ(animation.frames[3].tile_index, frame4.tile_index);
  EXPECT_EQ(animation.frames[3].duration, frame4.duration);
}

// tactile::core::remove_animation_frame
TEST(Animation, RemoveAnimationFrame)
{
  Registry registry {};
  const auto tile_entity = make_tile(registry, TileIndex {10});

  constexpr AnimationFrame frame1 {TileIndex {1}, std::chrono::milliseconds {10}};
  constexpr AnimationFrame frame2 {TileIndex {2}, std::chrono::milliseconds {20}};
  constexpr AnimationFrame frame3 {TileIndex {3}, std::chrono::milliseconds {30}};

  ASSERT_TRUE(add_animation_frame(registry, tile_entity, 0, frame1).has_value());
  ASSERT_TRUE(add_animation_frame(registry, tile_entity, 1, frame2).has_value());
  ASSERT_TRUE(add_animation_frame(registry, tile_entity, 2, frame3).has_value());

  const auto& animation = registry.get<CAnimation>(tile_entity);

  // [ frame1, frame2, frame3 ]
  ASSERT_EQ(animation.frames.size(), 3);
  EXPECT_EQ(animation.frames[0].tile_index, frame1.tile_index);
  EXPECT_EQ(animation.frames[0].duration, frame1.duration);
  EXPECT_EQ(animation.frames[1].tile_index, frame2.tile_index);
  EXPECT_EQ(animation.frames[1].duration, frame2.duration);
  EXPECT_EQ(animation.frames[2].tile_index, frame3.tile_index);
  EXPECT_EQ(animation.frames[2].duration, frame3.duration);

  // => [ frame1, frame2 ]
  EXPECT_TRUE(remove_animation_frame(registry, tile_entity, 2).has_value());
  ASSERT_EQ(animation.frames.size(), 2);
  EXPECT_EQ(animation.frames[0].tile_index, frame1.tile_index);
  EXPECT_EQ(animation.frames[0].duration, frame1.duration);
  EXPECT_EQ(animation.frames[1].tile_index, frame2.tile_index);
  EXPECT_EQ(animation.frames[1].duration, frame2.duration);

  // => [ frame2 ]
  EXPECT_TRUE(remove_animation_frame(registry, tile_entity, 0).has_value());
  ASSERT_EQ(animation.frames.size(), 1);
  EXPECT_EQ(animation.frames[0].tile_index, frame2.tile_index);
  EXPECT_EQ(animation.frames[0].duration, frame2.duration);

  // => [ ]
  EXPECT_TRUE(registry.has<CAnimation>(tile_entity));
  EXPECT_TRUE(remove_animation_frame(registry, tile_entity, 0).has_value());
  EXPECT_FALSE(registry.has<CAnimation>(tile_entity));
}

}  // namespace
}  // namespace tactile::core
