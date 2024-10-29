// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <memory>  // unique_ptr

#include "tactile/base/prelude.hpp"
#include "tactile/base/runtime/plugin.hpp"
#include "tactile/null_renderer/api.hpp"
#include "tactile/null_renderer/null_renderer.hpp"

namespace tactile {

/**
 * Manages the null renderer plugin.
 *
 * \note
 * This plugin is only intended for testing purposes.
 */
class TACTILE_NULL_RENDERER_API NullRendererPlugin final : public IPlugin
{
 public:
  void load(IRuntime* runtime) override;

  void unload() override;

 private:
  IRuntime* m_runtime {};
  std::unique_ptr<NullRenderer> m_renderer {};
};

extern "C"
{
  TACTILE_NULL_RENDERER_API auto tactile_make_plugin() -> IPlugin*;
  TACTILE_NULL_RENDERER_API void tactile_free_plugin(IPlugin* plugin);
}

}  // namespace tactile
