// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/document_manager.hpp"

#include <utility>  // move

#include "tactile/base/container/lookup.hpp"
#include "tactile/base/document/document.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map.hpp"

namespace tactile::core {

auto DocumentManager::create_and_open_map(const MapSpec& spec)
    -> std::expected<UUID, ErrorCode>
{
  auto document = MapDocument::make(spec);
  if (!document.has_value()) {
    return std::unexpected {document.error()};
  }

  const auto document_uuid = document->get_uuid();

  auto [iter, did_insert] =
      mDocuments.try_emplace(document_uuid,
                             std::make_unique<MapDocument>(std::move(*document)));

  // NB: A UUID collision should be exceptionally rare, but we check regardless.
  if (!did_insert) {
    TACTILE_CORE_ERROR("Could not store map document");
    return std::unexpected {ErrorCode::kBadState};
  }

  mOpenDocuments.push_back(document_uuid);
  mHistories.try_emplace(document_uuid, mCommandCapacity);

  mActiveDocument = document_uuid;

  return document_uuid;
}

auto DocumentManager::create_and_open_map(IRenderer& renderer, const ir::Map& ir_map)
    -> std::expected<UUID, ErrorCode>
{
  auto document = MapDocument::make(renderer, ir_map);
  if (!document.has_value()) {
    return std::unexpected {document.error()};
  }

  const auto document_uuid = document->get_uuid();

  auto [iter, did_insert] =
      mDocuments.try_emplace(document_uuid,
                             std::make_unique<MapDocument>(std::move(*document)));

  // NB: A UUID collision should be exceptionally rare, but we check regardless.
  if (!did_insert) {
    TACTILE_CORE_ERROR("Could not store map document");
    return std::unexpected {ErrorCode::kBadState};
  }

  mOpenDocuments.push_back(document_uuid);
  mHistories.try_emplace(document_uuid, mCommandCapacity);

  mActiveDocument = document_uuid;

  return document_uuid;
}

auto DocumentManager::get_document(const UUID& uuid) -> IDocument&
{
  return *lookup_in(mDocuments, uuid);
}

auto DocumentManager::get_document(const UUID& uuid) const -> const IDocument&
{
  return *lookup_in(mDocuments, uuid);
}

auto DocumentManager::get_current_document() -> IDocument*
{
  if (!mActiveDocument.is_null()) {
    return &get_document(mActiveDocument);
  }

  return nullptr;
}

auto DocumentManager::get_current_document() const -> const IDocument*
{
  if (!mActiveDocument.is_null()) {
    return &get_document(mActiveDocument);
  }

  return nullptr;
}

auto DocumentManager::get_open_documents() const -> const std::vector<UUID>&
{
  return mOpenDocuments;
}

void DocumentManager::set_command_capacity(const std::size_t capacity)
{
  TACTILE_CORE_DEBUG("Setting command capacity to {}", capacity);
  mCommandCapacity = capacity;

  for (auto& [document_uuid, command_stack] : mHistories) {
    command_stack.set_capacity(mCommandCapacity);
  }
}

auto DocumentManager::command_capacity() const -> std::size_t
{
  return mCommandCapacity;
}

auto DocumentManager::get_history(const UUID& uuid) -> CommandStack&
{
  return lookup_in(mHistories, uuid);
}

auto DocumentManager::get_history(const UUID& uuid) const -> const CommandStack&
{
  return lookup_in(mHistories, uuid);
}

auto DocumentManager::is_map_active() const -> bool
{
  if (!mActiveDocument.is_null()) {
    const auto& document = get_document(mActiveDocument);
    const auto& registry = document.get_registry();
    const auto& document_info = registry.get<CDocumentInfo>();
    return is_map(registry, document_info.root);
  }

  return false;
}

}  // namespace tactile::core
