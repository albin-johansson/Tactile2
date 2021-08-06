#pragma once

#include "core/commands/command_id.hpp"
#include "core/commands/maps/mergeable_map_command.hpp"

namespace Tactile {

/// \addtogroup core
/// \{

class MapDocument;

class RemoveRowCmd final : public MergeableMapCommand
{
 public:
  explicit RemoveRowCmd(NotNull<MapDocument*> document);

  void Undo() override;

  void Redo() override;

  [[nodiscard]] auto GetId() const noexcept -> int override
  {
    return CommandId::RemoveRow;
  }
};

/// \} End of group core

}  // namespace Tactile
