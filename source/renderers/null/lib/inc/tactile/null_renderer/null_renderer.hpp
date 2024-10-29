// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <unordered_map>  // unordered_map

#include "tactile/base/id.hpp"
#include "tactile/base/render/renderer.hpp"
#include "tactile/null_renderer/api.hpp"
#include "tactile/null_renderer/null_texture.hpp"

namespace tactile {

/**
 * A null renderer implementation.
 */
class TACTILE_NULL_RENDERER_API NullRenderer final : public IRenderer
{
 public:
  /**
   * Creates a renderer.
   *
   * \param window The associated window.
   */
  explicit NullRenderer(IWindow* window);

  [[nodiscard]]
  auto begin_frame() -> bool override;

  void end_frame() override;

  [[nodiscard]]
  auto load_texture(const std::filesystem::path& image_path)
      -> std::expected<TextureID, ErrorCode> override;

  void unload_texture(TextureID id) override;

  [[nodiscard]]
  auto find_texture(TextureID id) const -> const ITexture* override;

  void try_reload_fonts() override;

  [[nodiscard]]
  auto can_reload_fonts() const -> bool override;

  [[nodiscard]]
  auto get_window() -> IWindow* override;

  [[nodiscard]]
  auto get_window() const -> const IWindow* override;

  [[nodiscard]]
  auto get_imgui_context() -> ImGuiContext* override;

  void process_event(const SDL_Event& event) override;

  [[nodiscard]]
  auto get_options() -> const RendererOptions& override;

 private:
  RendererOptions m_options;
  IWindow* m_window;
  std::unordered_map<TextureID, NullTexture> m_textures;
  TextureID m_next_texture_id;
};

}  // namespace tactile
