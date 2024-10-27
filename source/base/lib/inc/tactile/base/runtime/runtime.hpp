// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstddef>  // size_t
#include <cstdint>  // uint32_t

#include "tactile/base/io/compress/compression_format_id.hpp"
#include "tactile/base/io/save/save_format_id.hpp"
#include "tactile/base/prelude.hpp"

namespace tactile {

class IWindow;
class IRenderer;
class ICompressionFormat;
class ISaveFormat;
struct RendererOptions;

namespace log {
class Logger;
}  // namespace log

/**
 * Interface that defines the primary API used by dynamic Tactile modules.
 */
class IRuntime
{
 public:
  TACTILE_INTERFACE_CLASS(IRuntime);

  using imgui_malloc_fn = void*(std::size_t, void*);
  using imgui_free_fn = void(void*, void*);

  /**
   * Initializes the application window.
   *
   * \param flags Additional flags forwarded to the \c SDL_CreateWindow
   *              function. Some flags are automatically added, the one thing
   *              renderer plugins must submit here is the flag that tells
   *              SDL which type of window to create. For example,
   *              \c SDL_WINDOW_OPENGL for an OpenGL window.
   */
  virtual void init_window(std::uint32_t flags) = 0;

  /**
   * Sets the associated renderer.
   *
   * \param renderer The renderer to use.
   */
  virtual void set_renderer(IRenderer* renderer) = 0;

  /**
   * Sets the compression format implementation for a given format.
   *
   * \param id     The compression format identifier.
   * \param format The compression format implementation. The compression format is
   *               unregistered for the given format if the pointer is null.
   */
  virtual void set_compression_format(CompressionFormatId id, ICompressionFormat* format) = 0;

  /**
   * Sets the save format implementation for a given format.
   *
   * \param id     The save format identifier.
   * \param format The save format implementation. The save format is
   *               unregistered for the given format if the pointer is null.
   */
  virtual void set_save_format(SaveFormatId id, ISaveFormat* format) = 0;

  /**
   * Returns the application window.
   *
   * \return
   * A possibly null pointer to the application window.
   */
  [[nodiscard]]
  virtual auto get_window() -> IWindow* = 0;

  /**
   * Returns the associated renderer.
   *
   * \return
   * A possibly null pointer to the renderer.
   */
  [[nodiscard]]
  virtual auto get_renderer() -> IRenderer* = 0;

  /**
   * Returns the compression format registered with a given format identifier.
   *
   * \param id The compression format identifier.
   *
   * \return
   * A possibly null pointer to the compression format.
   */
  [[nodiscard]]
  virtual auto get_compression_format(CompressionFormatId id) const
      -> const ICompressionFormat* = 0;

  /**
   * Returns the save format registered with a given format identifier.
   *
   * \param id The save format identifier.
   *
   * \return
   * A possibly null pointer to the save format.
   */
  [[nodiscard]]
  virtual auto get_save_format(SaveFormatId id) const -> const ISaveFormat* = 0;

  /**
   * Returns the allocation functions associated with the Dear ImGui context.
   *
   * \param[out] malloc_fn Pointer to the allocation function pointer to set.
   * \param[out] free_fn   Pointer to the deallocation function to set.
   * \param[out] user_data Pointer to the allocation user data to set.
   */
  virtual void get_imgui_allocator_functions(imgui_malloc_fn** malloc_fn,
                                             imgui_free_fn** free_fn,
                                             void** user_data) = 0;

  /**
   * Returns the configured renderer options.
   *
   * \return
   * The current renderer options.
   */
  [[nodiscard]]
  virtual auto get_renderer_options() const -> const RendererOptions& = 0;

  /**
   * Returns the associated runtime logger.
   *
   * \return
   * A pointer to the logger.
   */
  [[nodiscard]]
  virtual auto get_logger() const -> log::Logger* = 0;
};

}  // namespace tactile
