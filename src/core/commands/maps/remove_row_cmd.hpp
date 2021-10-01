#pragma once

#include <entt.hpp>  // registry

#include "common/ints.hpp"
#include "common/ref.hpp"
#include "core/commands/command.hpp"
#include "core/commands/command_id.hpp"
#include "map_command_cache.hpp"

namespace Tactile {

class RemoveRowCmd final : public ACommand
{
 public:
  explicit RemoveRowCmd(Ref<entt::registry> registry);

  void Undo() override;

  void Redo() override;

  [[nodiscard]] auto MergeWith(const ACommand& cmd) -> bool override;

  [[nodiscard]] auto GetId() const noexcept -> int override
  {
    return CommandId::RemoveRow;
  }

 private:
  Ref<entt::registry> mRegistry;
  MapCommandCache mCache;
  usize mRows{1};
};

}  // namespace Tactile
