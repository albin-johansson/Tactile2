#pragma once

#include <entt.hpp>  // registry

#include <tactile-base/layer_type.hpp>
#include "core/systems/layers/layer_snapshot.hpp"
#include "editor/commands/command.hpp"
#include "editor/commands/command_id.hpp"
#include <tactile-base/tactile_std.hpp>

namespace Tactile {

/// \addtogroup commands
/// \{

class AddLayerCmd final : public ACommand
{
 public:
  AddLayerCmd(Ref<entt::registry> registry, LayerType type);

  void Undo() override;

  void Redo() override;

  [[nodiscard]] auto GetId() const noexcept -> int override
  {
    return CommandId::AddLayer;
  }

 private:
  Ref<entt::registry> mRegistry;
  LayerType mLayerType;
  Maybe<LayerID> mLayerId;
  Maybe<Sys::LayerSnapshot> mLayerSnapshot;
};

/// \} End of group commands

}  // namespace Tactile
