// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmx_format/tiled_tmx_format_plugin.hpp"

#include <new>  // nothrow

#include "tactile/runtime/runtime.hpp"

namespace tactile {

void TiledTmxFormatPlugin::load(IRuntime& runtime)
{
  Runtime::log(LogLevel::kTrace, "Loading Tiled TMX format plugin");
}

void TiledTmxFormatPlugin::unload(IRuntime& runtime)
{
  Runtime::log(LogLevel::kTrace, "Unloading Tiled TMX format plugin");
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) TiledTmxFormatPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile
