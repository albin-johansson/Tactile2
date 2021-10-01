#pragma once

#include <entt.hpp>  // registry

#include "common/maybe.hpp"
#include "common/ref.hpp"
#include "common/tileset_id.hpp"
#include "core/commands/command.hpp"
#include "core/commands/command_id.hpp"
#include "core/components/texture.hpp"
#include "core/systems/tileset_snapshot.hpp"

namespace Tactile {

class AddTilesetCmd final : public ACommand
{
 public:
  AddTilesetCmd(Ref<entt::registry> registry,
                Texture texture,
                int tileWidth,
                int tileHeight);

  void Undo() override;

  void Redo() override;

  [[nodiscard]] auto GetId() const noexcept -> int override
  {
    return CommandId::AddTileset;
  }

 private:
  Ref<entt::registry> mRegistry;
  Texture mTexture;
  int mTileWidth;
  int mTileHeight;
  Maybe<TilesetID> mTilesetId;
  Maybe<Sys::TilesetSnapshot> mSnapshot;
};

}  // namespace Tactile
