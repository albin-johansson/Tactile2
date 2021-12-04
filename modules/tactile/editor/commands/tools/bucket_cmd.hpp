#pragma once

#include <vector>  // vector

#include <tactile_def.hpp>

#include <entt/entt.hpp>  // registry

#include "core/map_position.hpp"
#include "editor/commands/command.hpp"
#include "editor/commands/command_id.hpp"

namespace Tactile {

/// \addtogroup commands
/// \{

class BucketCmd final : public ACommand {
 public:
  BucketCmd(Ref<entt::registry> registry, MapPosition origin, TileID replacement);

  void Undo() override;

  void Redo() override;

  [[nodiscard]] auto GetId() const noexcept -> int override { return CommandId::Bucket; }

 private:
  Ref<entt::registry> mRegistry;
  LayerID mLayer;
  MapPosition mOrigin;
  TileID mReplacement;
  Maybe<TileID> mTarget;
  std::vector<MapPosition> mPositions;
};

/// \} End of group commands

}  // namespace Tactile
