// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/platform/filesystem.hpp"

#include <algorithm>  // replace
#include <cstdlib>    // system
#include <format>     // format

#include <SDL2/SDL.h>

#include "tactile/core/debug/generic_error.hpp"
#include "tactile/core/log/logger.hpp"
#include "tactile/core/platform/environment.hpp"
#include "tactile/core/util/string_conv.hpp"

namespace tactile {

auto open_directory_in_finder(const std::filesystem::path& dir)
    -> std::expected<void, std::error_code>
{
  if (!std::filesystem::is_directory(dir)) {
    return std::unexpected {make_error(GenericError::kInvalidParam)};
  }

  if constexpr (kOnMacos) {
    const auto cmd = std::format("open \"{}\"", dir.string());
    std::system(cmd.c_str());
  }
  else if constexpr (kOnWindows) {
    const auto cmd = std::format("explorer \"{}\"", dir.string());
    std::system(cmd.c_str());
  }
  else if constexpr (kOnLinux) {
    const auto cmd = std::format("xdg-open \"{}\"", dir.string());
    std::system(cmd.c_str());
  }
  else {
    TACTILE_LOG_ERROR("Cannot open finder on this platform");
    return std::unexpected {make_error(GenericError::kUnsupported)};
  }

  return {};
}

auto get_persistent_storage_directory()
    -> std::expected<std::filesystem::path, std::error_code>
{
  if (const auto* path = SDL_GetPrefPath("albin-johansson", "tactile")) {
    return std::filesystem::path {path};
  }

  TACTILE_LOG_ERROR("Could not determine persistent storage directory: {}", SDL_GetError());
  return std::unexpected {make_error(GenericError::kInvalidState)};
}

auto get_user_home_directory() -> std::expected<std::string, std::error_code>
{
  // On Unix platforms, HOME is something like '/Users/username'.
  // On Windows, USERPROFILE is something like 'C:\Users\username'.
  const auto* var_name = kOnWindows ? "USERPROFILE" : "HOME";
  return get_env(var_name);
}

auto get_imgui_ini_file_path() -> std::filesystem::path
{
  return "imgui.ini";
}

auto normalize_path(const std::filesystem::path& path) -> std::string
{
  auto str = path.string();
  std::ranges::replace(str, '\\', '/');
  return str;
}

auto has_prefix(const std::filesystem::path& path, const std::string_view prefix) -> bool
{
#if TACTILE_OS_WINDOWS
  return std::string_view {path.string().c_str()}.starts_with(prefix);
#else
  return std::string_view {path.c_str()}.starts_with(prefix);
#endif
}

auto strip_home_directory_prefix(const std::filesystem::path& path,
                                 const std::string_view home_dir)
    -> std::expected<std::string, std::error_code>
{
  if (has_prefix(path, home_dir)) {
    const NativeStringView path_view {path.c_str()};
    const auto path_without_home_dir = path_view.substr(home_dir.size());

    NativeString str {};
    str.reserve(1 + path_without_home_dir.size());
    str.push_back(TACTILE_NATIVE_CHAR('~'));
    str.insert(str.end(), path_without_home_dir.begin(), path_without_home_dir.end());

#if TACTILE_OS_WINDOWS
    return from_native_string(str);
#else
    return str;
#endif
  }

  return std::unexpected {make_error(GenericError::kInvalidParam)};
}

}  // namespace tactile
