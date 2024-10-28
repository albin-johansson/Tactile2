// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/layer/layer_common.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/object_layer.hpp"
#include "tactile/core/layer/tile_layer.hpp"
#include "tactile/core/test/ir_comparison.hpp"
#include "tactile/test_util/ir_presets.hpp"

namespace tactile::core {
namespace {

// tactile::core::make_layer [Registry&, const ir::Layer&]
TEST(LayerCommon, MakeLayerWithTileLayerIR)
{
  Registry registry {};

  const auto ir_layer = test::make_complex_ir_tile_layer(LayerID {11});

  const auto layer_id = make_layer(registry, ir_layer);
  ASSERT_TRUE(is_tile_layer(registry, layer_id));

  compare_layer(registry, layer_id, ir_layer);
}

// tactile::core::make_layer [Registry&, const ir::Layer&]
TEST(LayerCommon, MakeLayerWithObjectLayerIR)
{
  Registry registry {};

  ObjectID next_object_id {3};
  const auto ir_layer = test::make_complex_ir_object_layer(LayerID {42}, next_object_id);

  const auto layer_id = make_layer(registry, ir_layer);
  ASSERT_TRUE(is_object_layer(registry, layer_id));

  compare_layer(registry, layer_id, ir_layer);
}

// tactile::core::make_layer [Registry&, const ir::Layer&]
TEST(LayerCommon, MakeLayerWithGroupLayerIR)
{
  Registry registry {};

  LayerID next_layer_id {19};
  ObjectID next_object_id {43};
  const auto ir_layer = test::make_complex_ir_group_layer(next_layer_id, next_object_id);

  const auto layer_id = make_layer(registry, ir_layer);
  ASSERT_TRUE(is_group_layer(registry, layer_id));

  compare_layer(registry, layer_id, ir_layer);
}

}  // namespace
}  // namespace tactile::core
