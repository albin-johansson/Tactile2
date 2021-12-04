#pragma once

#include <tactile_def.hpp>

#include <imgui.h>

namespace Tactile::Scoped {

struct ID final {
  explicit ID(const void* ptr) { ImGui::PushID(ptr); }

  explicit ID(const CStr str) { ImGui::PushID(str); }

  explicit ID(const int id) { ImGui::PushID(id); }

  ~ID() { ImGui::PopID(); }
};

struct Disable final {
  explicit Disable(const bool disable = true) { ImGui::BeginDisabled(disable); }

  ~Disable() { ImGui::EndDisabled(); }
};

class Child final {
 public:
  explicit Child(const CStr id,
                 const ImVec2& size = {0, 0},
                 const bool border = false,
                 const ImGuiWindowFlags flags = 0)
      : mOpen{ImGui::BeginChild(id, size, border, flags)}
  {}

  ~Child() { ImGui::EndChild(); }

  [[nodiscard]] auto IsOpen() const noexcept -> bool { return mOpen; }

 private:
  bool mOpen{};
};

class Combo final {
 public:
  Combo(const CStr name, const CStr current) : mOpen{ImGui::BeginCombo(name, current)} {}

  ~Combo()
  {
    if (mOpen) {
      ImGui::EndCombo();
    }
  }

  [[nodiscard]] auto IsOpen() const noexcept -> bool { return mOpen; }

 private:
  bool mOpen{};
};

class TabBar final {
 public:
  explicit TabBar(const CStr name, const ImGuiTabBarFlags flags = 0)
      : mOpen{ImGui::BeginTabBar(name, flags)}
  {}

  ~TabBar()
  {
    if (mOpen) {
      ImGui::EndTabBar();
    }
  }

  [[nodiscard]] auto IsOpen() const noexcept -> bool { return mOpen; }

 private:
  bool mOpen{};
};

class TabItem final {
 public:
  explicit TabItem(const CStr name,
                   bool* open = nullptr,
                   const ImGuiTabItemFlags flags = 0)
      : mOpen{ImGui::BeginTabItem(name, open, flags)}
  {}

  ~TabItem()
  {
    if (mOpen) {
      ImGui::EndTabItem();
    }
  }

  [[nodiscard]] auto IsOpen() const noexcept -> bool { return mOpen; }

 private:
  bool mOpen{};
};

class Table final {
 public:
  Table(const CStr name, const int nColumns, const ImGuiTableFlags flags)
      : mOpen{ImGui::BeginTable(name, nColumns, flags)}
  {}

  ~Table()
  {
    if (mOpen) {
      ImGui::EndTable();
    }
  }

  [[nodiscard]] auto IsOpen() const noexcept -> bool { return mOpen; }

 private:
  bool mOpen{};
};

class Popup final {
 public:
  explicit Popup(const CStr name, const ImGuiWindowFlags flags = 0)
      : mOpen{ImGui::BeginPopup(name, flags)}
  {}

  [[nodiscard]] static auto ForItem(const CStr name, const ImGuiPopupFlags flags = 1)
      -> Popup
  {
    return Popup{ImGui::BeginPopupContextItem(name, flags)};
  }

  [[nodiscard]] static auto ForWindow(const CStr name, const ImGuiPopupFlags flags = 1)
      -> Popup
  {
    return Popup{ImGui::BeginPopupContextWindow(name, flags)};
  }

  ~Popup()
  {
    if (mOpen) {
      ImGui::EndPopup();
    }
  }

  [[nodiscard]] auto IsOpen() const noexcept -> bool { return mOpen; }

 private:
  bool mOpen{};

  explicit Popup(const bool open) : mOpen{open} {}
};

class ListBox final {
 public:
  explicit ListBox(const CStr label, const ImVec2& size = {0, 0})
      : mOpen{ImGui::BeginListBox(label, size)}
  {}

  ~ListBox()
  {
    if (mOpen) {
      ImGui::EndListBox();
    }
  }

  [[nodiscard]] auto IsOpen() const noexcept -> bool { return mOpen; }

 private:
  bool mOpen{};
};

}  // namespace Tactile::Scoped