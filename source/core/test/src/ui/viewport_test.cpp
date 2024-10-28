// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/ui/viewport.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"

namespace tactile::core {
namespace {

class ViewportTest : public testing::Test
{
 public:
  [[nodiscard]]
  auto make_viewport(const Float2& size = {500, 500}) -> EntityID
  {
    const auto entity = mRegistry.make_entity();

    auto& viewport = mRegistry.add<CViewport>(entity);
    viewport.pos = Float2 {0.0f, 0.0f};
    viewport.size = size;
    viewport.scale = 1.0f;

    return entity;
  }

 protected:
  Registry mRegistry {};
};

// tactile::core::is_viewport
TEST_F(ViewportTest, IsViewport)
{
  const auto entity = mRegistry.make_entity();
  EXPECT_FALSE(is_viewport(mRegistry, kInvalidEntity));
  EXPECT_FALSE(is_viewport(mRegistry, entity));

  mRegistry.add<CViewport>(entity);
  EXPECT_TRUE(is_viewport(mRegistry, entity));
}

// tactile::core::translate_viewport
TEST_F(ViewportTest, TranslateViewport)
{
  const auto viewport_entity = make_viewport();
  auto& viewport = mRegistry.get<CViewport>(viewport_entity);

  translate_viewport(viewport, Float2 {5.0f, 0.0f}, nullptr);
  EXPECT_EQ(viewport.pos, Float2(5.0f, 0.0f));

  translate_viewport(viewport, Float2 {1.0f, 0.0f}, nullptr);
  EXPECT_EQ(viewport.pos, Float2(6.0f, 0.0f));

  translate_viewport(viewport, Float2 {0.0f, -1.0f}, nullptr);
  EXPECT_EQ(viewport.pos, Float2(6.0f, -1.0f));

  translate_viewport(viewport, Float2 {10.0f, 10.0f}, nullptr);
  EXPECT_EQ(viewport.pos, Float2(16.0f, 9.0f));
}

// tactile::core::translate_viewport
// tactile::core::set_viewport_limits
TEST_F(ViewportTest, TranslateViewportWithLimits)
{
  const auto min_x = -8.0f;
  const auto max_x = 18.0f;
  const auto min_y = -3.0f;
  const auto max_y = 24.0f;

  const auto viewport_entity = make_viewport();

  set_viewport_limits(mRegistry,
                      viewport_entity,
                      Float2 {min_x, min_y},
                      Float2 {max_x, max_y});

  auto& viewport = mRegistry.get<CViewport>(viewport_entity);
  const auto* limits = mRegistry.find<CViewportLimits>(viewport_entity);

  // Should result in minimum valid offset.
  translate_viewport(viewport, Float2 {min_x, min_y}, limits);
  EXPECT_EQ(viewport.pos, (Float2 {min_x, min_y}));

  // One past minimum X.
  translate_viewport(viewport, Float2 {-1.0f, 0.0f}, limits);
  EXPECT_EQ(viewport.pos, (Float2 {min_x, min_y}));

  // One past minimum Y.
  translate_viewport(viewport, Float2 {0.0f, -1.0f}, limits);
  EXPECT_EQ(viewport.pos, (Float2 {min_x, min_y}));

  // Should result in maximum valid offset.
  translate_viewport(viewport, Float2 {-min_x, -min_y}, limits);
  translate_viewport(viewport, Float2 {max_x, max_y}, limits);
  EXPECT_EQ(viewport.pos, (Float2 {max_x, max_y}));

  // One past maximum X.
  translate_viewport(viewport, Float2 {1.0f, 0.0f}, limits);
  EXPECT_EQ(viewport.pos, (Float2 {max_x, max_y}));

  // One past maximum Y.
  translate_viewport(viewport, Float2 {0.0f, 1.0f}, limits);
  EXPECT_EQ(viewport.pos, (Float2 {max_x, max_y}));

  // Resetting the limits should clamp any existing offset.
  set_viewport_limits(mRegistry, viewport_entity, Float2 {-1.0f, -1.0f}, Float2 {1.0f, 1.0f});
  EXPECT_EQ(viewport.pos, (Float2 {1.0f, 1.0f}));
}

// tactile::core::increase_viewport_zoom
TEST_F(ViewportTest, IncreaseViewportZoom)
{
  const Float2 viewport_size {500, 500};
  const Float2 content_size {100, 100};

  const auto viewport_entity = make_viewport(viewport_size);
  auto& viewport = mRegistry.get<CViewport>(viewport_entity);

  EXPECT_EQ(viewport.scale, 1.0f);
  EXPECT_EQ(viewport.pos, (Float2 {0.0f, 0.0f}));
  EXPECT_EQ(viewport.size, viewport_size);

  increase_viewport_zoom(viewport, viewport.pos + content_size * 0.5f, nullptr);

  EXPECT_EQ(viewport.scale, 1.05f);
  EXPECT_NE(viewport.pos.x(), 0.0f);
  EXPECT_NE(viewport.pos.y(), 0.0f);
  EXPECT_EQ(viewport.size, viewport_size);
}

// tactile::core::decrease_viewport_zoom
TEST_F(ViewportTest, DecreaseViewportZoom)
{
  const Float2 viewport_size {300, 400};
  const Float2 content_size {80, 190};

  const auto viewport_entity = make_viewport(viewport_size);
  auto& viewport = mRegistry.get<CViewport>(viewport_entity);

  EXPECT_EQ(viewport.scale, 1.0f);
  EXPECT_EQ(viewport.pos, (Float2 {0.0f, 0.0f}));
  EXPECT_EQ(viewport.size, viewport_size);

  decrease_viewport_zoom(viewport, viewport.pos + content_size * 0.5f, nullptr);

  EXPECT_EQ(viewport.scale, 0.95f);
  EXPECT_NE(viewport.pos.x(), 0.0f);
  EXPECT_NE(viewport.pos.y(), 0.0f);
  EXPECT_EQ(viewport.size, viewport_size);
}

// tactile::core::center_viewport_over_content
TEST_F(ViewportTest, CenterViewportOverContent)
{
  const Float2 viewport_size {1000, 1000};
  const Float2 content_size {500, 500};

  const auto viewport_entity = make_viewport(viewport_size);
  auto& viewport = mRegistry.get<CViewport>(viewport_entity);

  center_viewport_over_content(viewport, content_size, nullptr);

  EXPECT_EQ(viewport.pos, (Float2 {-250, -250}));
}

// tactile::core::to_world_pos
TEST_F(ViewportTest, ToWorldPos)
{
  CViewport viewport {};
  viewport.pos = Float2 {-100, -50};
  viewport.size = Float2 {500, 300};
  viewport.scale = 1.0f;

  EXPECT_EQ(to_world_pos(viewport, Float2 {0, 0}), (Float2 {-100, -50}));
  EXPECT_EQ(to_world_pos(viewport, Float2 {10, 0}), (Float2 {-90, -50}));
  EXPECT_EQ(to_world_pos(viewport, Float2 {0, 10}), (Float2 {-100, -40}));
}

}  // namespace
}  // namespace tactile::core
