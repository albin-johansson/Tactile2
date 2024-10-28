// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/layer/group_layer.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer.hpp"
#include "tactile/core/layer/object_layer.hpp"
#include "tactile/core/layer/tile_layer.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {
namespace {

// Root
// ├── Layer 1
// ├── Group 1
// │   ├── Layer 2
// │   ├── Layer 3
// │   ├── Layer 4
// │   └── Group 2
// │       ├── Layer 5
// │       └── Layer 6
// ├── Group 3
// │   ├── Layer 7
// │   ├── Group 4
// │   │   ├── Layer 8
// │   │   ├── Group 5
// │   │   └── Group 6
// │   │       ├── Layer 9
// │   │       ├── Layer 10
// │   │       └── Layer 11
// │   ├── Layer 12
// │   └── Layer 13
// └── Layer 14
struct GroupLayerTestHierarchy final
{
  EntityID root_id;
  EntityID group1_id;
  EntityID group2_id;
  EntityID group3_id;
  EntityID group4_id;
  EntityID group5_id;
  EntityID group6_id;
  EntityID layer1_id;
  EntityID layer2_id;
  EntityID layer3_id;
  EntityID layer4_id;
  EntityID layer5_id;
  EntityID layer6_id;
  EntityID layer7_id;
  EntityID layer8_id;
  EntityID layer9_id;
  EntityID layer10_id;
  EntityID layer11_id;
  EntityID layer12_id;
  EntityID layer13_id;
  EntityID layer14_id;
};

class GroupLayerTest : public testing::Test
{
 public:
  [[nodiscard]]
  auto make_test_hierarchy() -> GroupLayerTestHierarchy
  {
    GroupLayerTestHierarchy hierarchy {};

    hierarchy.root_id = make_group_layer(mRegistry);

    hierarchy.group1_id = make_group_layer(mRegistry);
    hierarchy.group2_id = make_group_layer(mRegistry);
    hierarchy.group3_id = make_group_layer(mRegistry);
    hierarchy.group4_id = make_group_layer(mRegistry);
    hierarchy.group5_id = make_group_layer(mRegistry);
    hierarchy.group6_id = make_group_layer(mRegistry);

    hierarchy.layer1_id = make_object_layer(mRegistry);
    hierarchy.layer2_id = make_object_layer(mRegistry);
    hierarchy.layer3_id = make_object_layer(mRegistry);
    hierarchy.layer4_id = make_object_layer(mRegistry);
    hierarchy.layer5_id = make_object_layer(mRegistry);
    hierarchy.layer6_id = make_object_layer(mRegistry);
    hierarchy.layer7_id = make_object_layer(mRegistry);
    hierarchy.layer8_id = make_object_layer(mRegistry);
    hierarchy.layer9_id = make_object_layer(mRegistry);
    hierarchy.layer10_id = make_object_layer(mRegistry);
    hierarchy.layer11_id = make_object_layer(mRegistry);
    hierarchy.layer12_id = make_object_layer(mRegistry);
    hierarchy.layer13_id = make_object_layer(mRegistry);
    hierarchy.layer14_id = make_object_layer(mRegistry);

    {
      auto& root = mRegistry.get<CGroupLayer>(hierarchy.root_id);
      root.layers = {hierarchy.layer1_id,
                     hierarchy.group1_id,
                     hierarchy.group3_id,
                     hierarchy.layer14_id};
    }

    {
      auto& group1 = mRegistry.get<CGroupLayer>(hierarchy.group1_id);
      group1.layers = {hierarchy.layer2_id,
                       hierarchy.layer3_id,
                       hierarchy.layer4_id,
                       hierarchy.group2_id};
    }

    {
      auto& group2 = mRegistry.get<CGroupLayer>(hierarchy.group2_id);
      group2.layers = {hierarchy.layer5_id, hierarchy.layer6_id};
    }

    {
      auto& group3 = mRegistry.get<CGroupLayer>(hierarchy.group3_id);
      group3.layers = {hierarchy.layer7_id,
                       hierarchy.group4_id,
                       hierarchy.layer12_id,
                       hierarchy.layer13_id};
    }

    {
      auto& group4 = mRegistry.get<CGroupLayer>(hierarchy.group4_id);
      group4.layers = {hierarchy.layer8_id, hierarchy.group5_id, hierarchy.group6_id};
    }

    // Note, group 5 is empty.

    {
      auto& group6 = mRegistry.get<CGroupLayer>(hierarchy.group6_id);
      group6.layers = {hierarchy.layer9_id, hierarchy.layer10_id, hierarchy.layer11_id};
    }

    return hierarchy;
  }

 protected:
  Registry mRegistry {};
};

// tactile::core::is_group_layer
TEST_F(GroupLayerTest, IsGroupLayer)
{
  EXPECT_FALSE(is_group_layer(mRegistry, kInvalidEntity));
  EXPECT_FALSE(is_group_layer(mRegistry, mRegistry.make_entity()));

  const auto group_layer_entity = make_group_layer(mRegistry);
  EXPECT_TRUE(is_group_layer(mRegistry, group_layer_entity));
}

// tactile::core::make_group_layer
TEST_F(GroupLayerTest, MakeGroupLayer)
{
  const auto group_layer_entity = make_group_layer(mRegistry);

  EXPECT_TRUE(is_group_layer(mRegistry, group_layer_entity));
  EXPECT_TRUE(mRegistry.has<CMeta>(group_layer_entity));
  EXPECT_TRUE(mRegistry.has<CLayer>(group_layer_entity));
  EXPECT_TRUE(mRegistry.has<CGroupLayer>(group_layer_entity));

  const auto& meta = mRegistry.get<CMeta>(group_layer_entity);
  const auto& layer = mRegistry.get<CLayer>(group_layer_entity);
  const auto& group_layer = mRegistry.get<CGroupLayer>(group_layer_entity);

  EXPECT_EQ(meta.name, "");
  EXPECT_EQ(meta.properties.size(), 0);
  EXPECT_EQ(meta.components.size(), 0);

  EXPECT_EQ(layer.persistent_id, std::nullopt);
  EXPECT_EQ(layer.opacity, 1.0f);
  EXPECT_TRUE(layer.visible);

  EXPECT_EQ(group_layer.layers.size(), 0);
}

// tactile::core::destroy_group_layer
TEST_F(GroupLayerTest, DestroyEmptyGroupLayer)
{
  const auto group_layer_entity = make_group_layer(mRegistry);

  EXPECT_TRUE(mRegistry.is_valid(group_layer_entity));
  EXPECT_EQ(mRegistry.count<CMeta>(), 1);
  EXPECT_EQ(mRegistry.count<CLayer>(), 1);
  EXPECT_EQ(mRegistry.count<CGroupLayer>(), 1);
  EXPECT_EQ(mRegistry.count(), 3);

  destroy_group_layer(mRegistry, group_layer_entity);

  EXPECT_FALSE(mRegistry.is_valid(group_layer_entity));
  EXPECT_EQ(mRegistry.count<CMeta>(), 0);
  EXPECT_EQ(mRegistry.count<CLayer>(), 0);
  EXPECT_EQ(mRegistry.count<CGroupLayer>(), 0);
  EXPECT_EQ(mRegistry.count(), 0);
}

// tactile::core::destroy_group_layer
TEST_F(GroupLayerTest, DestroyGroupLayerWithStoredLayers)
{
  const auto group_layer_entity = make_group_layer(mRegistry);

  {
    constexpr Extent2D extent {5, 5};
    auto& group_layer = mRegistry.get<CGroupLayer>(group_layer_entity);

    const auto other_group_layer_entity = make_group_layer(mRegistry);
    auto& other_group_layer = mRegistry.get<CGroupLayer>(other_group_layer_entity);
    other_group_layer.layers.push_back(make_tile_layer(mRegistry, extent));

    group_layer.layers.push_back(make_tile_layer(mRegistry, extent));
    group_layer.layers.push_back(make_tile_layer(mRegistry, extent));
    group_layer.layers.push_back(make_object_layer(mRegistry));
    group_layer.layers.push_back(other_group_layer_entity);
  }

  EXPECT_TRUE(mRegistry.is_valid(group_layer_entity));
  EXPECT_EQ(mRegistry.count<CMeta>(), 6);
  EXPECT_EQ(mRegistry.count<CLayer>(), 6);
  EXPECT_EQ(mRegistry.count<CGroupLayer>(), 2);
  EXPECT_EQ(mRegistry.count<CTileLayer>(), 3);
  EXPECT_EQ(mRegistry.count<CDenseTileLayer>(), 3);
  EXPECT_EQ(mRegistry.count<CObjectLayer>(), 1);
  EXPECT_EQ(mRegistry.count(), 21);

  destroy_group_layer(mRegistry, group_layer_entity);

  EXPECT_FALSE(mRegistry.is_valid(group_layer_entity));
  EXPECT_EQ(mRegistry.count<CMeta>(), 0);
  EXPECT_EQ(mRegistry.count<CLayer>(), 0);
  EXPECT_EQ(mRegistry.count<CGroupLayer>(), 0);
  EXPECT_EQ(mRegistry.count<CTileLayer>(), 0);
  EXPECT_EQ(mRegistry.count<CDenseTileLayer>(), 0);
  EXPECT_EQ(mRegistry.count<CObjectLayer>(), 0);
  EXPECT_EQ(mRegistry.count(), 0);
}

// tactile::core::count_layers
TEST_F(GroupLayerTest, CountLayers)
{
  const auto tree = make_test_hierarchy();
  EXPECT_EQ(count_layers(mRegistry, tree.root_id), 20);
  EXPECT_EQ(count_layers(mRegistry, tree.group1_id), 6);
  EXPECT_EQ(count_layers(mRegistry, tree.group2_id), 2);
  EXPECT_EQ(count_layers(mRegistry, tree.group3_id), 10);
  EXPECT_EQ(count_layers(mRegistry, tree.group4_id), 6);
  EXPECT_EQ(count_layers(mRegistry, tree.group5_id), 0);
  EXPECT_EQ(count_layers(mRegistry, tree.group6_id), 3);
}

// tactile::core::find_parent_layer
TEST_F(GroupLayerTest, FindParentLayer)
{
  const auto tree = make_test_hierarchy();

  // Root
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer1_id), tree.root_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.group1_id), tree.root_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.group3_id), tree.root_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer14_id), tree.root_id);

  // Group 1
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer2_id), tree.group1_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer3_id), tree.group1_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer4_id), tree.group1_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.group2_id), tree.group1_id);

  // Group 2
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer5_id), tree.group2_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer6_id), tree.group2_id);

  // Group 3
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer7_id), tree.group3_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.group4_id), tree.group3_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer12_id), tree.group3_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer13_id), tree.group3_id);

  // Group 4
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer8_id), tree.group4_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.group5_id), tree.group4_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.group6_id), tree.group4_id);

  // Group 6
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer9_id), tree.group6_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer10_id), tree.group6_id);
  EXPECT_EQ(find_parent_layer(mRegistry, tree.root_id, tree.layer11_id), tree.group6_id);
}

// tactile::core::get_local_layer_index
TEST_F(GroupLayerTest, GetLocalLayerIndex)
{
  const auto tree = make_test_hierarchy();

  // Root
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer1_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group1_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group3_id), 2);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer14_id), 3);

  // Group 1
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer2_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer3_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer4_id), 2);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group2_id), 3);

  // Group 2
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer5_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer6_id), 1);

  // Group 3
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer7_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group4_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer12_id), 2);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer13_id), 3);

  // Group 4
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer8_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group5_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group6_id), 2);

  // Group 6
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer9_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer10_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer11_id), 2);
}

// tactile::core::get_global_layer_index
TEST_F(GroupLayerTest, GetGlobalLayerIndex)
{
  const auto tree = make_test_hierarchy();

  // clang-format off
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer1_id), 0);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.group1_id), 1);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer2_id), 2);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer3_id), 3);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer4_id), 4);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.group2_id), 5);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer5_id), 6);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer6_id), 7);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.group3_id), 8);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer7_id), 9);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.group4_id), 10);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer8_id), 11);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.group5_id), 12);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.group6_id), 13);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer9_id), 14);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer10_id), 15);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer11_id), 16);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer12_id), 17);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer13_id), 18);
  EXPECT_EQ(get_global_layer_index(mRegistry, tree.root_id, tree.layer14_id), 19);
  // clang-format on
}

// tactile::core::move_layer_up
TEST_F(GroupLayerTest, MoveLayerUp)
{
  const auto tree = make_test_hierarchy();

  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer2_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer3_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer4_id), 2);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group2_id), 3);
  ASSERT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.group2_id));

  move_layer_up(mRegistry, tree.root_id, tree.group2_id);

  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer2_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer3_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group2_id), 2);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer4_id), 3);
  ASSERT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.group2_id));

  move_layer_up(mRegistry, tree.root_id, tree.group2_id);

  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer2_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group2_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer3_id), 2);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer4_id), 3);
  ASSERT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.group2_id));

  move_layer_up(mRegistry, tree.root_id, tree.group2_id);

  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group2_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer2_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer3_id), 2);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer4_id), 3);
  EXPECT_FALSE(can_move_layer_up(mRegistry, tree.root_id, tree.group2_id));
}

// tactile::core::move_layer_down
TEST_F(GroupLayerTest, MoveLayerDown)
{
  const auto tree = make_test_hierarchy();

  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer8_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group5_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group6_id), 2);
  ASSERT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer8_id));

  move_layer_down(mRegistry, tree.root_id, tree.layer8_id);

  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group5_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer8_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group6_id), 2);
  ASSERT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer8_id));

  move_layer_down(mRegistry, tree.root_id, tree.layer8_id);

  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group5_id), 0);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.group6_id), 1);
  EXPECT_EQ(get_local_layer_index(mRegistry, tree.root_id, tree.layer8_id), 2);
  EXPECT_FALSE(can_move_layer_down(mRegistry, tree.root_id, tree.layer8_id));
}

// tactile::core::can_move_layer_up
TEST_F(GroupLayerTest, CanMoveLayerUp)
{
  const auto tree = make_test_hierarchy();

  // Root
  EXPECT_FALSE(can_move_layer_up(mRegistry, tree.root_id, tree.layer1_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.group1_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.group3_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.layer14_id));

  // Group 1
  EXPECT_FALSE(can_move_layer_up(mRegistry, tree.root_id, tree.layer2_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.layer3_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.layer4_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.group2_id));

  // Group 2
  EXPECT_FALSE(can_move_layer_up(mRegistry, tree.root_id, tree.layer5_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.layer6_id));

  // Group 3
  EXPECT_FALSE(can_move_layer_up(mRegistry, tree.root_id, tree.layer7_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.group4_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.layer12_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.layer13_id));

  // Group 4
  EXPECT_FALSE(can_move_layer_up(mRegistry, tree.root_id, tree.layer8_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.group5_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.group6_id));

  // Group 6
  EXPECT_FALSE(can_move_layer_up(mRegistry, tree.root_id, tree.layer9_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.layer10_id));
  EXPECT_TRUE(can_move_layer_up(mRegistry, tree.root_id, tree.layer11_id));
}

// tactile::core::can_move_layer_down
TEST_F(GroupLayerTest, CanMoveLayerDown)
{
  const auto tree = make_test_hierarchy();

  // Root
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer1_id));
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.group1_id));
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.group3_id));
  EXPECT_FALSE(can_move_layer_down(mRegistry, tree.root_id, tree.layer14_id));

  // Group 1
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer2_id));
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer3_id));
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer4_id));
  EXPECT_FALSE(can_move_layer_down(mRegistry, tree.root_id, tree.group2_id));

  // Group 2
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer5_id));
  EXPECT_FALSE(can_move_layer_down(mRegistry, tree.root_id, tree.layer6_id));

  // Group 3
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer7_id));
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.group4_id));
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer12_id));
  EXPECT_FALSE(can_move_layer_down(mRegistry, tree.root_id, tree.layer13_id));

  // Group 4
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer8_id));
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.group5_id));
  EXPECT_FALSE(can_move_layer_down(mRegistry, tree.root_id, tree.group6_id));

  // Group 6
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer9_id));
  EXPECT_TRUE(can_move_layer_down(mRegistry, tree.root_id, tree.layer10_id));
  EXPECT_FALSE(can_move_layer_down(mRegistry, tree.root_id, tree.layer11_id));
}

}  // namespace
}  // namespace tactile::core
