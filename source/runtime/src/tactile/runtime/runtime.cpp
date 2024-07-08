// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/runtime/runtime.hpp"

#include <cstdlib>    // malloc, free, EXIT_SUCCESS, EXIT_FAILURE
#include <exception>  // exception, set_terminate
#include <utility>    // move

#include <imgui.h>

#include "tactile/base/container/hash_map.hpp"
#include "tactile/base/container/maybe.hpp"
#include "tactile/core/debug/terminate.hpp"
#include "tactile/core/engine/engine.hpp"
#include "tactile/core/log/logger.hpp"
#include "tactile/core/log/set_log_scope.hpp"
#include "tactile/core/log/terminal_log_sink.hpp"
#include "tactile/core/numeric/random.hpp"
#include "tactile/core/platform/win32.hpp"
#include "tactile/core/tactile_editor.hpp"
#include "tactile/core/ui/common/style.hpp"
#include "tactile/runtime/protobuf_context.hpp"
#include "tactile/runtime/sdl_context.hpp"
#include "tactile/runtime/window.hpp"

namespace tactile {
namespace runtime_impl {

struct ImGuiContextDeleter final
{
  void operator()(ImGuiContext* context) noexcept
  {
    ImGui::DestroyContext(context);
  }
};

using UniqueImGuiContext = Unique<ImGuiContext, ImGuiContextDeleter>;

[[nodiscard]]
auto make_logger() -> Logger
{
  win32_enable_virtual_terminal_processing();

  auto terminal_sink = std::make_unique<TerminalLogSink>();
  terminal_sink->use_ansi_colors(true);

  Logger logger {};

  logger.set_reference_instant(SteadyClock::now());
  logger.set_min_level(LogLevel::kTrace);
  logger.add_sink(std::move(terminal_sink));

  return logger;
}

[[nodiscard]]
auto make_imgui_context() -> UniqueImGuiContext
{
  UniqueImGuiContext imgui_context {ImGui::CreateContext()};

  // NOLINTBEGIN(*-no-malloc)
  ImGui::SetAllocatorFunctions(
      [](const usize size, void*) { return std::malloc(size); },
      [](void* ptr, void*) noexcept { std::free(ptr); });
  ImGui::SetCurrentContext(imgui_context.get());
  // NOLINTEND(*-no-malloc)

  auto& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ui::apply_custom_style(ImGui::GetStyle());

  return imgui_context;
}

}  // namespace runtime_impl

struct Runtime::Data final
{
  ProtobufContext protobuf_context;
  SDLContext sdl_context;
  Logger logger;
  runtime_impl::UniqueImGuiContext imgui_context {};
  Optional<Window> window {};
  IRenderer* renderer {};
  HashMap<CompressionType, ICompressionProvider*> compression_providers {};
  Optional<TactileEditor> editor {};

  Data()
    : protobuf_context {},
      sdl_context {},
      logger {runtime_impl::make_logger()},
      imgui_context {runtime_impl::make_imgui_context()}
  {
    set_default_logger(&logger);
  }

  ~Data() noexcept
  {
    set_default_logger(nullptr);
  }

  TACTILE_DELETE_COPY(Data);
  TACTILE_DELETE_MOVE(Data);
};

Runtime::Runtime()
{
  std::set_terminate(&on_terminate);
  mData = std::make_unique<Data>();

  TACTILE_LOG_INFO("Tactile " TACTILE_VERSION_STRING);
  init_random_number_generator();
}

Runtime::~Runtime() noexcept = default;

void Runtime::init_window(const uint32 flags)
{
  if (auto window = Window::create(flags)) {
    mData->window = std::move(*window);
  }
}

void Runtime::set_renderer(IRenderer* renderer)
{
  mData->renderer = renderer;
}

void Runtime::set_compression_provider(const CompressionType format,
                                       ICompressionProvider* provider)
{
  if (provider != nullptr) {
    mData->compression_providers.insert_or_assign(format, provider);
  }
  else {
    mData->compression_providers.erase(format);
  }
}

auto Runtime::start() -> int
{
  auto& data = *mData;

  if (!data.window) {
    TACTILE_LOG_ERROR("Window has not been initialized");
    return EXIT_FAILURE;
  }

  if (!data.renderer) {
    TACTILE_LOG_ERROR("A renderer has not been installed");
    return EXIT_FAILURE;
  }

  auto* window = &data.window.value();
  auto* renderer = data.renderer;

  auto& app = data.editor.emplace(window, renderer);

  Engine engine {&app, renderer};
  engine.run();

  return EXIT_SUCCESS;
}

auto Runtime::get_window() -> IWindow*
{
  return mData->window.has_value() ? &mData->window.value() : nullptr;
}

auto Runtime::get_imgui_context() -> ImGuiContext*
{
  return mData->imgui_context.get();
}

}  // namespace tactile