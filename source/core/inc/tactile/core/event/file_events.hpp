// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/container/path.hpp"
#include "tactile/base/prelude.hpp"

namespace tactile {

/**
 * Event for saving the current document to disk.
 */
struct SaveEvent final
{};

/**
 * Event for saving the current document to a new location.
 */
struct SaveAsEvent final
{
  /** The chosen file path. */
  Path path;
};

/**
 * Event for reopening the last closed file.
 */
struct ReopenLastClosedFileEvent final
{};

/**
 * Event for clearing the file history.
 */
struct ClearFileHistoryEvent final
{};

/**
 * Event for closing the current document.
 */
struct CloseEvent final
{};

/**
 * Event for gracefully shutting down the application.
 */
struct QuitEvent final
{};

}  // namespace tactile
