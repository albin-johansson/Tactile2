// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/opengl/opengl_renderer.hpp"

#include <cstdlib>        // malloc, free
#include <list>           // list
#include <memory>         // unique_ptr, nothrow
#include <optional>       // optional
#include <unordered_map>  // unordered_map
#include <utility>        // move

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "tactile/base/render/window.hpp"
#include "tactile/base/util/scope_exit.hpp"
#include "tactile/opengl/logging.hpp"
#include "tactile/opengl/opengl_error.hpp"
#include "tactile/opengl/opengl_texture.hpp"

namespace tactile::gl {

struct GLContextDeleter final
{
  void operator()(void* context) noexcept
  {
    SDL_GL_DeleteContext(context);
  }
};

struct OpenGLRenderer::Data final  // NOLINT(*-member-init)
{
  RendererOptions options;
  ImGuiContext* imgui_context;
  IWindow* window;
  std::unique_ptr<void, GLContextDeleter> gl_context;
  ScopeExit imgui_context_deleter {};
  ScopeExit imgui_backend_impl_deleter {};
  ScopeExit imgui_renderer_impl_deleter {};
  std::unordered_map<TextureID, OpenGLTexture> textures;
  TextureID next_texture_id;
};

auto OpenGLRenderer::make(const RendererOptions& options, IWindow* window)
    -> std::expected<OpenGLRenderer, ErrorCode>
{
  if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO) {
    return std::unexpected {ErrorCode::kBadState};
  }

  if (!window) {
    return std::unexpected {ErrorCode::kBadParam};
  }

  OpenGLRenderer renderer {};
  auto& data = *renderer.m_data;

  data.options = options;
  data.window = window;
  data.next_texture_id = TextureID {1};

  data.gl_context.reset(SDL_GL_CreateContext(window->get_handle()));
  if (!data.gl_context) {
    return std::unexpected {ErrorCode::kBadInit};
  }

  if (!gladLoadGLLoader(&SDL_GL_GetProcAddress)) {
    return std::unexpected {ErrorCode::kBadInit};
  }

  data.imgui_context = ImGui::CreateContext();

  if (!data.imgui_context) {
    TACTILE_OPENGL_ERROR("Could not create ImGui context");
    return std::unexpected {ErrorCode::kBadInit};
  }

  data.imgui_context_deleter =
      ScopeExit {[ctx = data.imgui_context] { ImGui::DestroyContext(ctx); }};

  if (!ImGui_ImplSDL2_InitForOpenGL(window->get_handle(), data.imgui_context)) {
    return std::unexpected {ErrorCode::kBadInit};
  }

  data.imgui_backend_impl_deleter = ScopeExit {[] { ImGui_ImplSDL2_Shutdown(); }};

  if (!ImGui_ImplOpenGL3_Init("#version 410 core")) {
    return std::unexpected {ErrorCode::kBadInit};
  }

  data.imgui_renderer_impl_deleter = ScopeExit {[] { ImGui_ImplOpenGL3_Shutdown(); }};

  const auto preferred_swap_interval = options.use_vsync ? (options.limit_fps ? 1 : -1) : 0;
  TACTILE_OPENGL_TRACE("Preferred swap interval mode is {}", preferred_swap_interval);

  const auto set_swap_interval_result = SDL_GL_SetSwapInterval(preferred_swap_interval);
  if (set_swap_interval_result != 0 && options.use_vsync) {
    SDL_GL_SetSwapInterval(1);
  }

  const auto swap_interval = SDL_GL_GetSwapInterval();
  TACTILE_OPENGL_DEBUG("Swap interval is {}", swap_interval);

  return renderer;
}

OpenGLRenderer::OpenGLRenderer()
  : m_data {std::make_unique<Data>()}
{}

OpenGLRenderer::OpenGLRenderer(OpenGLRenderer&& other) noexcept = default;

auto OpenGLRenderer::operator=(OpenGLRenderer&& other) noexcept -> OpenGLRenderer& = default;

OpenGLRenderer::~OpenGLRenderer() noexcept = default;

auto OpenGLRenderer::begin_frame() -> bool
{
  ImGui_ImplSDL2_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  const auto& io = ImGui::GetIO();
  glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  return true;
}

void OpenGLRenderer::end_frame()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if constexpr (kOnMacos) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }
  SDL_GL_SwapWindow(m_data->window->get_handle());
}

auto OpenGLRenderer::load_texture(const std::filesystem::path& image_path)
    -> std::expected<TextureID, ErrorCode>
{
  auto texture = OpenGLTexture::load(image_path, m_data->options);
  if (!texture.has_value()) {
    return std::unexpected {texture.error()};
  }

  auto& data = *m_data;

  const auto texture_id = data.next_texture_id;
  ++data.next_texture_id.value;

  m_data->textures.try_emplace(texture_id, std::move(*texture));
  return texture_id;
}

void OpenGLRenderer::unload_texture(const TextureID id)
{
  m_data->textures.erase(id);
}

auto OpenGLRenderer::find_texture(const TextureID id) const -> const ITexture*
{
  if (const auto iter = m_data->textures.find(id); iter != m_data->textures.end()) {
    return &iter->second;
  }

  return nullptr;
}

void OpenGLRenderer::try_reload_fonts()
{
  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_CreateFontsTexture();
}

auto OpenGLRenderer::can_reload_fonts() const -> bool
{
  return true;
}

auto OpenGLRenderer::get_window() -> IWindow*
{
  return m_data->window;
}

auto OpenGLRenderer::get_window() const -> const IWindow*
{
  return m_data->window;
}

auto OpenGLRenderer::get_imgui_context() -> ImGuiContext*
{
  return ImGui::GetCurrentContext();
}

void OpenGLRenderer::process_event(const SDL_Event& event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);
}

auto OpenGLRenderer::get_options() -> const RendererOptions&
{
  return m_data->options;
}

}  // namespace tactile::gl
