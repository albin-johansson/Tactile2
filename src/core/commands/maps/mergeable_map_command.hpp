#pragma once

#include <string>  // string

#include "core/commands/maps/map_command.hpp"

namespace Tactile {

/// \addtogroup core
/// \{

class MergeableMapCommand : public MapCommand
{
 public:
  MergeableMapCommand(NotNull<MapDocument*> document, std::string text);

  auto MergeWith(const ACommand& cmd) -> bool override;

 protected:
  [[nodiscard]] auto GetAmount() const noexcept -> int
  {
    return mAmount;
  }

 private:
  int mAmount{1};
};

/// \} End of group core

}  // namespace Tactile
