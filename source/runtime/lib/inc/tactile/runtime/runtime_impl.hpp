// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstdint>  // uint32_t
#include <memory>   // unique_ptr

#include "tactile/base/io/compress/compression_format_id.hpp"
#include "tactile/base/io/save/save_format_id.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/runtime/runtime.hpp"
#include "tactile/runtime/api.hpp"

struct ImGuiContext;

namespace tactile::runtime {

struct CommandLineOptions;

/**
 * Implements the runtime interface.
 */
class TACTILE_RUNTIME_API RuntimeImpl final : public IRuntime
{
 public:
  TACTILE_DELETE_COPY(RuntimeImpl);
  TACTILE_DELETE_MOVE(RuntimeImpl);

  /**
   * Performs basic initialization of the runtime.
   *
   * \details
   * This constructor performs the below steps. The only things that must be
   * manually configured is the application window (via \c init_window) and the
   * renderer (via \c set_renderer). These are expected to be configured by
   * renderer plugins, in their \c IPlugin::load implementations. \n
   *
   * - Installs a custom terminate handler. \n
   * - Initializes the Protobuf library. \n
   * - Initializes The SDL library. \n
   * - Creates and installs a core logger.
   *
   * \param options The provided command line options.
   */
  explicit RuntimeImpl(const CommandLineOptions& options);

  ~RuntimeImpl() noexcept override;

  void init_window(std::uint32_t flags) override;

  void set_renderer(IRenderer* renderer) override;

  void set_compression_format(CompressionFormatId id, ICompressionFormat* format) override;

  void set_save_format(SaveFormatId id, ISaveFormat* format) override;

  [[nodiscard]]
  auto get_window() -> IWindow* override;

  [[nodiscard]]
  auto get_renderer() -> IRenderer* override;

  [[nodiscard]]
  auto get_compression_format(CompressionFormatId id) const
      -> const ICompressionFormat* override;

  [[nodiscard]]
  auto get_save_format(SaveFormatId id) const -> const ISaveFormat* override;

  void get_imgui_allocator_functions(imgui_malloc_fn** malloc_fn,
                                     imgui_free_fn** free_fn,
                                     void** user_data) override;

  [[nodiscard]]
  auto get_renderer_options() const -> const RendererOptions& override;

  [[nodiscard]]
  auto get_logger() const -> log::Logger* override;

 private:
  struct Data;
  std::unique_ptr<Data> m_data;
};

}  // namespace tactile::runtime
