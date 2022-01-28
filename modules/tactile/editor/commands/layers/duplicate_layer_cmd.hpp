#pragma once

#include <entt/entt.hpp>  // registry

#include "editor/commands/command.hpp"
#include "editor/commands/command_id.hpp"
#include "tactile_def.hpp"

namespace tactile {

class DuplicateLayerCmd final : public ACommand {
 public:
  DuplicateLayerCmd(RegistryRef registry, LayerID id);

  void Undo() override;

  void Redo() override;

  [[nodiscard]] auto GetId() const noexcept -> int override
  {
    return CommandId::DuplicateLayer;
  }

 private:
  RegistryRef mRegistry;
  LayerID mLayerId;
  Maybe<LayerID> mNewLayerId;
};

}  // namespace tactile