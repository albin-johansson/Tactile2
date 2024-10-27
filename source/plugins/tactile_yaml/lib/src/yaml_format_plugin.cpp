// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/yaml/yaml_format_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/yaml/logging.hpp"

namespace tactile {

void YamlFormatPlugin::load(IRuntime* runtime)
{
  mRuntime = runtime;
  yaml_format::set_logger(runtime->get_logger());
}

void YamlFormatPlugin::unload()
{
  yaml_format::set_logger(nullptr);
  mRuntime = nullptr;
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) YamlFormatPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile
