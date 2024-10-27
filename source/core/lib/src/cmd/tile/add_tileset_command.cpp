// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/tile/add_tileset_command.hpp"

#include <exception>  // exception
#include <utility>    // move

#include "tactile/core/debug/validation.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/tile/tileset.hpp"

namespace tactile::core {

AddTilesetCommand::AddTilesetCommand(MapDocument* document, TilesetSpec spec)
  : m_document {require_not_null(document, "null document")},
    m_spec {std::move(spec)},
    m_tileset_id {kInvalidEntity},
    m_tileset_was_added {false}
{}

AddTilesetCommand::~AddTilesetCommand() noexcept
{
  try {
    if (!m_tileset_was_added && m_tileset_id != kInvalidEntity) {
      auto& registry = m_document->get_registry();
      destroy_tileset(registry, m_tileset_id);
    }
  }
  catch (const std::exception& error) {
    TACTILE_CORE_ERROR("Unexpected exception in AddTilesetCommand destructor: {}",
                      error.what());
  }
}

void AddTilesetCommand::undo()
{
  auto& registry = m_document->get_registry();

  const auto& document_info = registry.get<CDocumentInfo>();
  const auto map_id = document_info.root;

  remove_tileset_from_map(registry, map_id, m_tileset_id);

  m_tileset_was_added = false;
}

void AddTilesetCommand::redo()
{
  auto& registry = m_document->get_registry();

  const auto& document_info = registry.get<CDocumentInfo>();
  const auto map_id = document_info.root;

  if (m_tileset_id == kInvalidEntity) {
    m_tileset_id = add_tileset_to_map(registry, map_id, m_spec).value();
  }
  else {
    auto& map = registry.get<CMap>(map_id);
    map.attached_tilesets.push_back(m_tileset_id);
    map.active_tileset = m_tileset_id;
  }

  m_tileset_was_added = true;
}

}  // namespace tactile::core
