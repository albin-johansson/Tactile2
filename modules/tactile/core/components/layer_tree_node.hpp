#pragma once

#include <vector>  // vector

#include <tactile_def.hpp>

#include <entt/entt.hpp>  // entity

namespace Tactile {

struct LayerTreeNode final
{
  usize index{};                       ///< Local index, i.e. relative to tree siblings.
  std::vector<entt::entity> children;  ///< All associated child nodes.
};

}  // namespace Tactile