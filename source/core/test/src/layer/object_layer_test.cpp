// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/layer/object_layer.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/layer/object.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {
namespace {

// tactile::core::is_object_layer
TEST(ObjectLayer, IsObjectLayer)
{
  Registry registry {};
  EXPECT_FALSE(is_object_layer(registry, kInvalidEntity));
  EXPECT_FALSE(is_object_layer(registry, registry.make_entity()));
  EXPECT_TRUE(is_object_layer(registry, make_object_layer(registry)));
}

// tactile::core::make_object_layer
TEST(ObjectLayer, MakeObjectLayer)
{
  Registry registry {};
  const auto object_layer_entity = make_object_layer(registry);

  EXPECT_TRUE(is_object_layer(registry, object_layer_entity));

  EXPECT_TRUE(registry.has<CMeta>(object_layer_entity));
  EXPECT_TRUE(registry.has<CLayer>(object_layer_entity));
  EXPECT_TRUE(registry.has<CObjectLayer>(object_layer_entity));

  const auto& meta = registry.get<CMeta>(object_layer_entity);
  const auto& layer = registry.get<CLayer>(object_layer_entity);
  const auto& object_layer = registry.get<CObjectLayer>(object_layer_entity);

  EXPECT_EQ(meta.name, "");
  EXPECT_EQ(meta.properties.size(), 0);
  EXPECT_EQ(meta.components.size(), 0);

  EXPECT_EQ(layer.opacity, 1.0f);
  EXPECT_EQ(layer.persistent_id, std::nullopt);
  EXPECT_TRUE(layer.visible);

  EXPECT_EQ(object_layer.objects.size(), 0);
}

// tactile::core::destroy_object_layer
TEST(ObjectLayer, DestroyObjectLayer)
{
  Registry registry {};

  const auto object_layer_entity = make_object_layer(registry);
  auto& object_layer = registry.get<CObjectLayer>(object_layer_entity);

  const auto object1 = make_object(registry, ObjectID {1}, ObjectType::kPoint);
  const auto object2 = make_object(registry, ObjectID {2}, ObjectType::kRect);

  object_layer.objects.push_back(object1);
  object_layer.objects.push_back(object2);

  EXPECT_EQ(registry.count<CMeta>(), 3);
  EXPECT_EQ(registry.count<CLayer>(), 1);
  EXPECT_EQ(registry.count<CObjectLayer>(), 1);
  EXPECT_EQ(registry.count<CObject>(), 2);
  EXPECT_EQ(registry.count(), 7);
  EXPECT_TRUE(registry.is_valid(object_layer_entity));
  EXPECT_TRUE(registry.is_valid(object1));
  EXPECT_TRUE(registry.is_valid(object2));

  destroy_object_layer(registry, object_layer_entity);

  EXPECT_EQ(registry.count<CMeta>(), 0);
  EXPECT_EQ(registry.count<CLayer>(), 0);
  EXPECT_EQ(registry.count<CObjectLayer>(), 0);
  EXPECT_EQ(registry.count<CObject>(), 0);
  EXPECT_EQ(registry.count(), 0);
  EXPECT_FALSE(registry.is_valid(object_layer_entity));
  EXPECT_FALSE(registry.is_valid(object1));
  EXPECT_FALSE(registry.is_valid(object2));
}

}  // namespace
}  // namespace tactile::core
