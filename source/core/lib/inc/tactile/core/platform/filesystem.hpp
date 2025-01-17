// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>    // expected
#include <filesystem>  // path
#include <optional>    // optional
#include <string>      // string

#include "tactile/base/debug/error_code.hpp"
#include "tactile/base/prelude.hpp"

namespace tactile::core {

/**
 * Attempts to open a directory in the operating system file explorer.
 *
 * \param dir The directory to open.
 *
 * \return
 * Nothing if the function tried to open the directory; an error code otherwise.
 */
auto open_directory_in_finder(const std::filesystem::path& dir)
    -> std::expected<void, ErrorCode>;

/**
 * Returns the file path to the persistent storage directory associated with the
 * app.
 *
 * \return
 * A directory path if successful; an error code otherwise.
 */
[[nodiscard]]
auto get_persistent_storage_directory() -> std::expected<std::filesystem::path, ErrorCode>;

/**
 * Returns the file path to the user home directory.
 *
 * \return
 * A directory path if successful; an empty optional otherwise.
 */
[[nodiscard]]
auto get_user_home_directory() -> std::optional<std::string>;

/**
 * Returns the path to the associated \c imgui.ini file.
 *
 * \return
 * A file path.
 */
[[nodiscard]]
auto get_imgui_ini_file_path() -> std::filesystem::path;

/**
 * Converts a path to a string that is guaranteed to use forward slash
 * characters.
 *
 * \details
 * This function is useful when saving paths to files in a portable way. Since
 * all relevant operating systems understand forward slashes, even if some
 * operating systems prefer backslashes (most notably Windows).
 *
 * \param path A file path to convert.
 *
 * \return
 * A file path string that uses forward slashes.
 */
[[nodiscard]]
auto normalize_path(const std::filesystem::path& path) -> std::string;

/**
 * Indicates whether a path features a given prefix.
 *
 * \param path   The path to check.
 * \param prefix The prefix to look for.
 *
 * \return
 * True if the path features the specified prefix; false otherwise.
 */
[[nodiscard]]
auto has_prefix(const std::filesystem::path& path, std::string_view prefix) -> bool;

/**
 * Replaces the user home directory prefix in a file path with '~'.
 *
 * \param path     The file path.
 * \param home_dir The user home directory path.
 *
 * \return
 * A string with a shortened home directory prefix; or an error code if not
 * applicable.
 */
[[nodiscard]]
auto strip_home_directory_prefix(const std::filesystem::path& path, std::string_view home_dir)
    -> std::expected<std::string, ErrorCode>;

}  // namespace tactile::core
