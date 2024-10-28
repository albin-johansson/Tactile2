// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/layer/layer.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {
namespace {

// tactile::core::is_layer
TEST(Layer, IsLayer)
{
  Registry registry {};
  EXPECT_FALSE(is_layer(registry, kInvalidEntity));
  EXPECT_FALSE(is_layer(registry, registry.make_entity()));
  EXPECT_TRUE(is_layer(registry, make_unspecialized_layer(registry)));
}

// tactile::core::make_unspecialized_layer
TEST(Layer, MakeUnspecializedLayer)
{
  Registry registry {};

  const auto layer_entity = make_unspecialized_layer(registry);
  EXPECT_TRUE(is_layer(registry, layer_entity));

  const auto& meta = registry.get<CMeta>(layer_entity);
  const auto& layer = registry.get<CLayer>(layer_entity);

  EXPECT_EQ(meta.name, "");
  EXPECT_EQ(meta.properties.size(), 0);
  EXPECT_EQ(meta.components.size(), 0);

  EXPECT_EQ(layer.persistent_id, std::nullopt);
  EXPECT_EQ(layer.opacity, 1.0f);
  EXPECT_TRUE(layer.visible);
}

}  // namespace
}  // namespace tactile::core
