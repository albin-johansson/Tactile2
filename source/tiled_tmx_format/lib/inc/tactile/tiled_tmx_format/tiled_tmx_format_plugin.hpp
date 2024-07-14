// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/prelude.hpp"
#include "tactile/runtime/plugin.hpp"
#include "tactile/tiled_tmx_format/api.hpp"

namespace tactile {

class TACTILE_TMX_FORMAT_API TiledTmxFormatPlugin final : public IPlugin
{
 public:
  void load(Runtime& runtime) override;

  void unload(Runtime& runtime) override;
};

extern "C"
{
  TACTILE_TMX_FORMAT_API auto tactile_make_plugin() -> IPlugin*;
  TACTILE_TMX_FORMAT_API void tactile_free_plugin(IPlugin* plugin);
}

}  // namespace tactile
