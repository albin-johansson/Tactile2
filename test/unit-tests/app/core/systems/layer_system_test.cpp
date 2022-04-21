#include "core/systems/layers/layer_system.hpp"

#include <gtest/gtest.h>

#include "core/components/layers.hpp"
#include "core/systems/layers/layer_tree_system.hpp"
#include "core/systems/registry_system.hpp"

using namespace tactile;

namespace {

constexpr entt::entity null_entity = entt::null;

}  // namespace

TEST(LayerSystem, SortLayers)
{
  auto registry = sys::make_document_registry();
  auto& active = registry.ctx().at<comp::ActiveLayer>();

  const auto a = sys::new_tile_layer(registry);
  const auto b = sys::new_object_layer(registry);
  const auto c = sys::new_group_layer(registry);

  active.entity = c;
  const auto d = sys::new_tile_layer(registry);
  const auto e = sys::new_tile_layer(registry);
  const auto f = sys::new_group_layer(registry);

  active.entity = f;
  const auto g = sys::new_tile_layer(registry);
  const auto h = sys::new_tile_layer(registry);

  active.entity = c;
  const auto i = sys::new_tile_layer(registry);
  const auto j = sys::new_tile_layer(registry);

  active.entity = entt::null;
  const auto k = sys::new_object_layer(registry);

  /*
   - A
   - B
   - C
     - D
     - E
     - F
       - G
       - H
     - I
     - J
   - K
   */

  ASSERT_EQ(0, sys::layer_local_index(registry, a));
  ASSERT_EQ(1, sys::layer_local_index(registry, b));
  ASSERT_EQ(2, sys::layer_local_index(registry, c));
  {
    ASSERT_EQ(0, sys::layer_local_index(registry, d));
    ASSERT_EQ(1, sys::layer_local_index(registry, e));
    ASSERT_EQ(2, sys::layer_local_index(registry, f));
    {
      ASSERT_EQ(0, sys::layer_local_index(registry, g));
      ASSERT_EQ(1, sys::layer_local_index(registry, h));
    }

    ASSERT_EQ(3, sys::layer_local_index(registry, i));
    ASSERT_EQ(4, sys::layer_local_index(registry, j));
  }

  ASSERT_EQ(3, sys::layer_local_index(registry, k));
}

TEST(LayerSystem, RemoveLayer)
{
  auto registry = sys::make_document_registry();
  auto& active = registry.ctx().at<comp::ActiveLayer>();

  const auto a = sys::new_tile_layer(registry);
  const auto b = sys::new_group_layer(registry);

  active.entity = b;
  const auto c = sys::new_object_layer(registry);
  const auto d = sys::new_object_layer(registry);
  const auto e = sys::new_group_layer(registry);

  active.entity = e;
  const auto f = sys::new_tile_layer(registry);
  const auto g = sys::new_tile_layer(registry);

  active.entity = entt::null;
  const auto h = sys::new_tile_layer(registry);

  ASSERT_EQ(0, sys::layer_local_index(registry, a));
  ASSERT_EQ(1, sys::layer_local_index(registry, b));
  {
    ASSERT_EQ(0, sys::layer_local_index(registry, c));
    ASSERT_EQ(1, sys::layer_local_index(registry, d));
    ASSERT_EQ(2, sys::layer_local_index(registry, e));
    {
      ASSERT_EQ(0, sys::layer_local_index(registry, f));
      ASSERT_EQ(1, sys::layer_local_index(registry, g));
    }
  }
  ASSERT_EQ(2, sys::layer_local_index(registry, h));

  sys::remove_layer(registry, a);
  ASSERT_EQ(0, sys::layer_local_index(registry, b));
  {
    ASSERT_EQ(0, sys::layer_local_index(registry, c));
    ASSERT_EQ(1, sys::layer_local_index(registry, d));
    ASSERT_EQ(2, sys::layer_local_index(registry, e));
    {
      ASSERT_EQ(0, sys::layer_local_index(registry, f));
      ASSERT_EQ(1, sys::layer_local_index(registry, g));
    }
  }
  ASSERT_EQ(1, sys::layer_local_index(registry, h));

  sys::remove_layer(registry, d);
  ASSERT_EQ(0, sys::layer_local_index(registry, b));
  {
    ASSERT_EQ(0, sys::layer_local_index(registry, c));
    ASSERT_EQ(1, sys::layer_local_index(registry, e));
    {
      ASSERT_EQ(0, sys::layer_local_index(registry, f));
      ASSERT_EQ(1, sys::layer_local_index(registry, g));
    }
  }
  ASSERT_EQ(1, sys::layer_local_index(registry, h));

  sys::remove_layer(registry, b);
  ASSERT_EQ(0, sys::layer_local_index(registry, h));
  ASSERT_EQ(1, registry.storage<comp::Layer>().size());

  // Make sure all child layers were destroyed
  ASSERT_FALSE(registry.valid(a));
  ASSERT_FALSE(registry.valid(c));
  ASSERT_FALSE(registry.valid(d));
  ASSERT_FALSE(registry.valid(e));
  ASSERT_FALSE(registry.valid(f));
  ASSERT_FALSE(registry.valid(g));
  ASSERT_TRUE(registry.valid(h));
}

TEST(LayerSystem, SelectLayer)
{
  // TODO
}

TEST(LayerSystem, DuplicateLayer)
{
  // TODO improve this test

  auto registry = sys::make_document_registry();

  const auto a = sys::new_tile_layer(registry);
  auto& layerA = registry.get<comp::Layer>(a);
  layerA.opacity = 0.5f;
  layerA.visible = false;

  const auto b = sys::duplicate_layer(registry, a);
  const auto& layerB = registry.get<comp::Layer>(a);

  ASSERT_EQ(0, registry.get<comp::LayerTreeNode>(a).index);
  ASSERT_EQ(1, registry.get<comp::LayerTreeNode>(b).index);

  ASSERT_EQ(layerA.opacity, layerB.opacity);
  ASSERT_EQ(layerA.visible, layerB.visible);
}

TEST(LayerSystem, FindLayer)
{
  auto registry = sys::make_document_registry();
  ASSERT_EQ(null_entity, sys::find_layer(registry, 1));

  const auto a = sys::new_tile_layer(registry);
  const auto b = sys::new_object_layer(registry);

  ASSERT_EQ(a, sys::find_layer(registry, 1));
  ASSERT_EQ(b, sys::find_layer(registry, 2));
}
