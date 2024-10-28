// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <optional>  // optional

#include "tactile/base/prelude.hpp"
#include "tactile/base/runtime/plugin.hpp"
#include "tactile/opengl/api.hpp"
#include "tactile/opengl/opengl_renderer.hpp"

namespace tactile::gl {

/**
 * Manages the OpenGL renderer plugin.
 */
class TACTILE_OPENGL_API OpenGLRendererPlugin final : public IPlugin
{
 public:
  void load(IRuntime* runtime) override;

  void unload() override;

 private:
  IRuntime* m_runtime {};
  std::optional<OpenGLRenderer> m_renderer {};
};

extern "C"
{
  TACTILE_OPENGL_API auto tactile_make_plugin() -> IPlugin*;

  TACTILE_OPENGL_API void tactile_free_plugin(IPlugin* plugin);
}

}  // namespace tactile::gl
