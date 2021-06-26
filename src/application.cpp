#include "application.hpp"

#include <utility>  // move

#include "gui/update_gui.hpp"
#include "gui/widgets/menus/menu_bar_widget.hpp"
#include "gui/widgets/viewport/viewport_widget.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

namespace tactile {

Application::Application(cen::window&& window, cen::gl_context&& context)
    : mWindow{std::move(window)}
    , mContext{std::move(context)}
    , mModel{std::make_unique<Model>()}
{
  // clang-format off
  mDispatcher.sink<AddMapEvent>().connect<&Application::OnAddMapEvent>(this);
  mDispatcher.sink<OpenMapEvent>().connect<&Application::OnOpenMapEvent>(this);
  mDispatcher.sink<AddTilesetEvent>().connect<&Application::OnAddTilesetEvent>(this);
  mDispatcher.sink<UndoEvent>().connect<&Application::OnUndoEvent>(this);
  mDispatcher.sink<RedoEvent>().connect<&Application::OnRedoEvent>(this);
  mDispatcher.sink<CenterViewportEvent>().connect<&Application::OnCenterViewportEvent>(this);
  mDispatcher.sink<SelectLayerEvent>().connect<&Application::OnSelectLayerEvent>(this);
  mDispatcher.sink<QuitEvent>().connect<&Application::OnQuitEvent>(this);
  // clang-format on

  const auto a = mModel->AddMap();
  const auto b = mModel->AddMap();
  mModel->SelectMap(b);

  auto* document = mModel->GetDocument(b);
  auto& map = document->GetMap();
  map.AddTileLayer();
  map.AddObjectLayer();
  map.SelectLayer(1_layer);
}

auto Application::Run() -> int
{
  const auto& io = ImGui::GetIO();

  while (!mQuit)
  {
    PollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(mWindow.get());

    ImGui::NewFrame();
    UpdateFrame();
    ImGui::EndFrame();

    ImGui::Render();

    glViewport(0,
               0,
               static_cast<int>(io.DisplaySize.x),
               static_cast<int>(io.DisplaySize.y));
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    cen::gl::swap(mWindow);
  }

  mWindow.hide();
  return 0;
}

void Application::PollEvents()
{
  cen::event event;
  while (event.poll())
  {
    ImGui_ImplSDL2_ProcessEvent(event.data());

    if (event.is<cen::quit_event>())
    {
      mQuit = true;
      break;
    }
    else if (const auto* keyEvent = event.try_get<cen::keyboard_event>())
    {
      if (keyEvent->pressed())
      {
        const auto ctrl = keyEvent->is_active(cen::key_modifier::ctrl);
        const auto shift = keyEvent->is_active(cen::key_modifier::shift);
        const auto alt = keyEvent->is_active(cen::key_modifier::alt);

        if (ctrl && alt)
        {
          OnCtrlAltKeyStroke(keyEvent->scan());
        }
        else if (ctrl)
        {
          OnCtrlKeyStroke(keyEvent->scan());
        }
      }
    }
  }
}

void Application::UpdateFrame()
{
  mDispatcher.update();
  UpdateGui(*mModel, mDispatcher);
}

void Application::OnCtrlKeyStroke(const cen::scan_code key)
{
  if (key == cen::scancodes::g)
  {
    ToggleMapGrid();
  }
  else if (key == cen::scancodes::o)
  {
    EnableOpenMapDialog();
  }
  else if (key == cen::scancodes::t)
  {
    EnableTilesetDialog();
  }
  else if (key == cen::scancodes::space)
  {
    CenterViewport();
  }
}

void Application::OnCtrlAltKeyStroke(const cen::scan_code key)
{
  if (key == cen::scancodes::s)
  {
    EnableSettingsDialog();
  }
}

void Application::OnAddMapEvent(const AddMapEvent& event)
{
  cen::log::info("Application::OnAddMapEvent");
}

void Application::OnOpenMapEvent(const OpenMapEvent& event)
{
  cen::log::info("Application::OnOpenMapEvent > %s",
                 event.path.filename().string().c_str());
}

void Application::OnAddTilesetEvent(const AddTilesetEvent& event)
{
  cen::log::info(
      "Application::OnAddTilesetEvent > Path: \"%s\", Tile width: %i, Tile "
      "height: %i",
      event.path.string().c_str(),
      event.tile_width,
      event.tile_height);
}

void Application::OnUndoEvent(const UndoEvent& event)
{
  cen::log::info("Application::OnUndoEvent");
}

void Application::OnRedoEvent(const RedoEvent& event)
{
  cen::log::info("Application::OnRedoEvent");
}

void Application::OnCenterViewportEvent(const CenterViewportEvent& event)
{
  CenterViewport();
}

void Application::OnSelectLayerEvent(const SelectLayerEvent& event)
{
  if (auto* document = mModel->GetActiveDocument())
  {
    auto& map = document->GetMap();
    map.SelectLayer(event.id);
    CENTURION_LOG_DEBUG("Selected layer %i!", event.id.get());
  }
}

void Application::OnQuitEvent(const QuitEvent& event)
{
  mQuit = true;
}

}  // namespace tactile
