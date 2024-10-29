// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/layer/layer_common.hpp"

#include <stdexcept>  // runtime_error, invalid_argument

#include "tactile/base/io/save/ir.hpp"
#include "tactile/core/debug/assert.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer.hpp"
#include "tactile/core/layer/object.hpp"
#include "tactile/core/layer/object_layer.hpp"
#include "tactile/core/layer/tile_layer.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {

auto make_layer(Registry& registry, const ir::Layer& ir_layer) -> EntityID
{
  EntityID layer_id {kInvalidEntity};

  switch (ir_layer.type) {
    case LayerType::kTileLayer: {
      layer_id = make_tile_layer(registry, ir_layer.extent);

      for (Extent2D::value_type row = 0; row < ir_layer.extent.rows; ++row) {
        for (Extent2D::value_type col = 0; col < ir_layer.extent.cols; ++col) {
          const auto tile_id = ir_layer.tiles[row][col];
          set_layer_tile(registry, layer_id, Index2D {.x = col, .y = row}, tile_id);
        }
      }

      break;
    }
    case LayerType::kObjectLayer: {
      layer_id = make_object_layer(registry);

      auto& object_layer = registry.get<CObjectLayer>(layer_id);
      object_layer.objects.reserve(ir_layer.objects.size());

      for (const auto& ir_object : ir_layer.objects) {
        object_layer.objects.push_back(make_object(registry, ir_object));
      }

      break;
    }
    case LayerType::kGroupLayer: {
      layer_id = make_group_layer(registry);

      auto& group_layer = registry.get<CGroupLayer>(layer_id);
      group_layer.layers.reserve(ir_layer.layers.size());

      for (const auto& ir_sublayer : ir_layer.layers) {
        group_layer.layers.push_back(make_layer(registry, ir_sublayer));
      }

      break;
    }
    default: throw std::runtime_error {"invalid layer type"};
  }

  auto& layer = registry.get<CLayer>(layer_id);
  layer.persistent_id = ir_layer.id;
  layer.opacity = ir_layer.opacity;
  layer.visible = ir_layer.visible;

  convert_ir_metadata(registry, layer_id, ir_layer.meta);

  TACTILE_ASSERT(is_layer(registry, layer_id));
  return layer_id;
}

void destroy_layer(Registry& registry, const EntityID layer_entity)
{
  TACTILE_ASSERT(is_tile_layer(registry, layer_entity) ||    //
                 is_object_layer(registry, layer_entity) ||  //
                 is_group_layer(registry, layer_entity));

  if (registry.has<CTileLayer>(layer_entity)) {
    destroy_tile_layer(registry, layer_entity);
  }
  else if (registry.has<CObjectLayer>(layer_entity)) {
    destroy_object_layer(registry, layer_entity);
  }
  else if (registry.has<CGroupLayer>(layer_entity)) {
    destroy_group_layer(registry, layer_entity);
  }
}

auto copy_layer(Registry& registry, const EntityID source_layer_entity, LayerID& next_layer_id)
    -> EntityID
{
  TACTILE_ASSERT(is_layer(registry, source_layer_entity));

  const auto& source_meta = registry.get<CMeta>(source_layer_entity);
  const auto& source_layer = registry.get<CLayer>(source_layer_entity);

  const auto new_layer_entity = make_unspecialized_layer(registry);

  auto& new_meta = registry.add<CMeta>(new_layer_entity, source_meta);
  new_meta.name += "*";

  auto& new_layer = registry.add<CLayer>(new_layer_entity, source_layer);
  new_layer.persistent_id = next_layer_id++;

  if (is_tile_layer(registry, source_layer_entity)) {
    const auto& source_tile_layer = registry.get<CTileLayer>(source_layer_entity);
    registry.add<CTileLayer>(new_layer_entity, source_tile_layer);

    if (registry.has<CDenseTileLayer>(source_layer_entity)) {
      const auto& source_dense_tile_layer = registry.get<CDenseTileLayer>(source_layer_entity);
      registry.add<CDenseTileLayer>(new_layer_entity, source_dense_tile_layer);
    }
    else {
      const auto& source_sparse_tile_layer =
          registry.get<CSparseTileLayer>(source_layer_entity);
      registry.add<CSparseTileLayer>(new_layer_entity, source_sparse_tile_layer);
    }
  }
  else if (is_object_layer(registry, source_layer_entity)) {
    const auto& source_object_layer = registry.get<CObjectLayer>(source_layer_entity);

    auto& new_object_layer = registry.add<CObjectLayer>(new_layer_entity);
    new_object_layer.objects.reserve(source_object_layer.objects.size());

    for (const auto source_object_id : source_object_layer.objects) {
      const auto new_object_id = copy_object(registry, source_object_id);
      new_object_layer.objects.push_back(new_object_id);
    }
  }
  else if (is_group_layer(registry, source_layer_entity)) {
    const auto& source_group_layer = registry.get<CGroupLayer>(source_layer_entity);

    auto& new_group_layer = registry.add<CGroupLayer>(new_layer_entity);
    new_group_layer.layers.reserve(source_group_layer.layers.size());

    for (const auto source_nested_layer_id : source_group_layer.layers) {
      const auto new_nested_layer_id =
          copy_layer(registry, source_nested_layer_id, next_layer_id);
      new_group_layer.layers.push_back(new_nested_layer_id);
    }
  }
  else {
    throw std::invalid_argument {"invalid layer entity"};
  }

  TACTILE_ASSERT(is_layer(registry, new_layer_entity));
  return new_layer_entity;
}

}  // namespace tactile::core
