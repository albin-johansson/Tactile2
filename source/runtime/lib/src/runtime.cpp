// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/runtime/runtime.hpp"

#include <chrono>         // steady_clock
#include <cstdlib>        // malloc, free, EXIT_SUCCESS, EXIT_FAILURE
#include <exception>      // exception, set_terminate
#include <optional>       // optional
#include <unordered_map>  // unordered_map
#include <utility>        // move

#include <imgui.h>

#include "tactile/base/render/renderer.hpp"
#include "tactile/core/debug/terminate.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/numeric/random.hpp"
#include "tactile/core/platform/win32.hpp"
#include "tactile/core/ui/common/style.hpp"
#include "tactile/log/file_log_sink.hpp"
#include "tactile/log/terminal_log_sink.hpp"
#include "tactile/runtime/command_line_options.hpp"
#include "tactile/runtime/logging.hpp"
#include "tactile/runtime/protobuf_context.hpp"
#include "tactile/runtime/sdl_context.hpp"
#include "tactile/runtime/window.hpp"

namespace tactile::runtime {
namespace {

[[nodiscard]]
auto _imgui_malloc(const std::size_t bytes, void*) -> void*
{
  return std::malloc(bytes);
}

void _imgui_free(void* memory, void*)
{
  std::free(memory);
}

[[nodiscard]]
auto _make_logger(const log::LogLevel log_level) -> log::Logger
{
  log::Logger logger {};

  auto terminal_sink = std::make_unique<log::TerminalLogSink>();
  terminal_sink->set_use_ansi_colors(true);

  auto file_sink = std::make_unique<log::FileLogSink>("log.txt");

  logger.add_sink(std::move(terminal_sink));
  logger.add_sink(std::move(file_sink));

  logger.set_reference_instant(log::Logger::clock_type::now());
  logger.set_log_level(log_level);
  logger.set_flush_level(log::LogLevel::kError);

  return logger;
}

void _log_command_line_options(const CommandLineOptions& options)
{
  TACTILE_RUNTIME_TRACE("renderer: {}", options.renderer_backend);
  TACTILE_RUNTIME_TRACE("load_zlib: {}", options.load_zlib);
  TACTILE_RUNTIME_TRACE("load_zstd: {}", options.load_zstd);
  TACTILE_RUNTIME_TRACE("load_yaml_format: {}", options.load_yaml_format);
  TACTILE_RUNTIME_TRACE("load_tiled_tmj_format: {}", options.load_tiled_tmj_format);
  TACTILE_RUNTIME_TRACE("load_tiled_tmx_format: {}", options.load_tiled_tmx_format);
  TACTILE_RUNTIME_TRACE("load_godot_tscn_format: {}", options.load_godot_tscn_format);
  TACTILE_RUNTIME_TRACE("texture_filter_mode: {}",
                        options.renderer_options.texture_filter_mode);
  TACTILE_RUNTIME_TRACE("use_mipmaps: {}", options.renderer_options.use_mipmaps);
  TACTILE_RUNTIME_TRACE("use_vsync: {}", options.renderer_options.use_vsync);
  TACTILE_RUNTIME_TRACE("limit_fps: {}", options.renderer_options.limit_fps);
  TACTILE_RUNTIME_TRACE("vulkan_validation: {}", options.renderer_options.vulkan_validation);
}

}  // namespace

struct Runtime::Data final
{
  RendererOptions renderer_options;
  ProtobufContext protobuf_context;
  SDLContext sdl_context;
  log::Logger logger;
  std::optional<Window> window {};
  IRenderer* renderer {};
  std::unordered_map<CompressionFormatId, ICompressionFormat*> compression_formats {};
  std::unordered_map<SaveFormatId, ISaveFormat*> save_formats {};

  explicit Data(const CommandLineOptions& options)
    : renderer_options {options.renderer_options},
      logger {_make_logger(options.log_level)}
  {
    set_logger(&logger);
    core::set_logger(&logger);

    _log_command_line_options(options);
    TACTILE_RUNTIME_DEBUG("Tactile " TACTILE_VERSION_STRING);
  }

  ~Data() noexcept
  {
    core::set_logger(nullptr);
    set_logger(nullptr);
  }

  TACTILE_DELETE_COPY(Data);
  TACTILE_DELETE_MOVE(Data);
};

Runtime::Runtime(const CommandLineOptions& options)
{
  std::set_terminate(&core::on_terminate);
  mData = std::make_unique<Data>(options);

  core::init_random_number_generator();
}

Runtime::~Runtime() noexcept = default;

void Runtime::init_window(const std::uint32_t flags)
{
  if (auto window = Window::create(flags)) {
    core::win32_use_immersive_dark_mode(window->get_handle());
    mData->window = std::move(*window);
  }
}

void Runtime::set_renderer(IRenderer* renderer)
{
  mData->renderer = renderer;
}

void Runtime::set_compression_format(const CompressionFormatId id, ICompressionFormat* format)
{
  if (format != nullptr) {
    mData->compression_formats.insert_or_assign(id, format);
  }
  else {
    mData->compression_formats.erase(id);
  }
}

void Runtime::set_save_format(const SaveFormatId id, ISaveFormat* format)
{
  if (format != nullptr) {
    mData->save_formats.insert_or_assign(id, format);
  }
  else {
    mData->save_formats.erase(id);
  }
}

auto Runtime::get_window() -> IWindow*
{
  return mData->window.has_value() ? &mData->window.value() : nullptr;
}

auto Runtime::get_renderer() -> IRenderer*
{
  return mData->renderer;
}

auto Runtime::get_compression_format(const CompressionFormatId id) const
    -> const ICompressionFormat*
{
  const auto iter = mData->compression_formats.find(id);

  if (iter != mData->compression_formats.end()) {
    return iter->second;
  }

  return nullptr;
}

auto Runtime::get_save_format(const SaveFormatId id) const -> const ISaveFormat*
{
  const auto iter = mData->save_formats.find(id);

  if (iter != mData->save_formats.end()) {
    return iter->second;
  }

  return nullptr;
}

void Runtime::get_imgui_allocator_functions(imgui_malloc_fn** malloc_fn,
                                            imgui_free_fn** free_fn,
                                            void** user_data)
{
  if (malloc_fn) {
    *malloc_fn = &_imgui_malloc;
  }

  if (free_fn) {
    *free_fn = &_imgui_free;
  }

  if (user_data) {
    *user_data = nullptr;
  }
}

auto Runtime::get_renderer_options() const -> const RendererOptions&
{
  return mData->renderer_options;
}

auto Runtime::get_logger() const -> log::Logger*
{
  return &mData->logger;
}

}  // namespace tactile::runtime
