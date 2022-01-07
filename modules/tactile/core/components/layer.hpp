#pragma once

#include <vector>  // vector

#include <entt/entt.hpp>  // entity, null
#include <tactile_def.hpp>

namespace Tactile {

/**
 * \brief Context component that keeps track of the selected layer.
 */
struct ActiveLayer final
{
  entt::entity entity{entt::null};  ///< The currently active layer, can be null.
};

/**
 * \brief Component used to represent the layer hierarchy.
 */
struct LayerTreeNode final
{
  usize index{};                       ///< Local index, i.e. relative to tree siblings.
  std::vector<entt::entity> children;  ///< All associated child nodes.
};

/**
 * \brief Component that provides common aspects of layers.
 *
 * \details This component is used by all different kinds of layers.
 *
 * \see TileLayer
 * \see ObjectLayer
 * \see GroupLayer
 * \see ActiveLayer
 */
struct Layer final
{
  LayerID id{};         ///< Unique layer ID.
  LayerType type{};     ///< The specific layer type.
  float opacity{1.0f};  ///< Opacity of the layer, in the range [0, 1].
  bool visible{true};   ///< Whether or not the layer is rendered.
};

/**
 * \brief Component that represents plain tile layers.
 */
struct TileLayer final
{
  TileMatrix matrix;  ///< The associated matrix of tile identifiers.
};

/**
 * \brief Component that represents object layers.
 *
 * \see Object
 */
struct ObjectLayer final
{
  std::vector<entt::entity> objects;  ///< The associated object entities.
};

/// \brief Tag component used to denote group layers.
struct GroupLayer final
{};

}  // namespace Tactile
