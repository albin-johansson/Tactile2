#pragma once

#include <entt/entt.hpp>  // registry

#include "editor/commands/command.hpp"
#include "editor/commands/command_id.hpp"
#include "map_command_cache.hpp"
#include "tactile_def.hpp"

namespace tactile {

class RemoveRowCmd final : public command_base {
 public:
  explicit RemoveRowCmd(registry_ref registry);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto merge_with(const command_base& cmd) -> bool override;

  [[nodiscard]] auto id() const noexcept -> int override
  {
    return CommandId::RemoveRow;
  }

 private:
  registry_ref mRegistry;
  MapCommandCache mCache;
  usize mRows{1};
};

}  // namespace tactile
