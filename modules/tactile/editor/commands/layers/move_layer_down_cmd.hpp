#pragma once

#include <entt/entt.hpp>  // registry
#include <tactile_def.hpp>

#include "editor/commands/command.hpp"
#include "editor/commands/command_id.hpp"

namespace Tactile {

class MoveLayerDownCmd final : public ACommand {
 public:
  MoveLayerDownCmd(Ref<entt::registry> registry, LayerID id);

  void Undo() override;

  void Redo() override;

  [[nodiscard]] auto GetId() const noexcept -> int override
  {
    return CommandId::MoveLayerDown;
  }

 private:
  Ref<entt::registry> mRegistry;
  LayerID mLayerId;
};

}  // namespace Tactile
