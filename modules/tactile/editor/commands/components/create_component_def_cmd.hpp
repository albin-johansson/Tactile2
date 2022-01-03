#pragma once

#include <string>  // string

#include <tactile_def.hpp>

#include "core/components/component.hpp"
#include "editor/commands/command.hpp"
#include "editor/commands/command_id.hpp"

namespace Tactile {

class CreateComponentDefCmd final : public ACommand {
 public:
  CreateComponentDefCmd(RegistryRef registry, std::string name);

  void Undo() override;

  void Redo() override;

  [[nodiscard]] auto GetId() const noexcept -> int override
  {
    return CommandId::CreateComponentDef;
  }

 private:
  RegistryRef mRegistry;
  std::string mName;
  Maybe<ComponentID> mComponentId;
};

}  // namespace Tactile