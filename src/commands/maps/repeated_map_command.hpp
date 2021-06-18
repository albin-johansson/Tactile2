#pragma once

#include "forward_declare.hpp"
#include "map_command.hpp"
#include "map_position.hpp"
#include "not_null.hpp"

TACTILE_FORWARD_DECLARE(tactile::core, MapDocument)

namespace tactile::cmd {

class RepeatedMapCommand : public MapCommand
{
 public:
  RepeatedMapCommand(NotNull<core::MapDocument*> document,
                     const QString& name);

  auto mergeWith(const QUndoCommand* other) -> bool final;

 protected:
  [[nodiscard]] auto Times() const noexcept -> int
  {
    return mTimes;
  }

 private:
  int mTimes{1};
};

}  // namespace tactile::cmd
