// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "document_manager.hpp"

#include <algorithm>  // any_of
#include <utility>    // move

#include "core/tile/tileset_bundle.hpp"
#include "model/document/map_document.hpp"
#include "model/document/tileset_document.hpp"
#include "tactile/core/debug/assert.hpp"
#include "tactile/core/debug/exception.hpp"
#include "tactile/core/util/lookup.hpp"

namespace tactile {

void DocumentManager::each(const DocVisitorFn& op) const
{
  for (const auto& [id, doc] : mDocuments) {
    op(doc);
  }
}

void DocumentManager::each_open(const IdVisitorFn& op) const
{
  for (const auto& id : mOpenDocuments) {
    op(id);
  }
}

void DocumentManager::add_map_document(Shared<MapDocument> document)
{
  if (!document) {
    throw Exception {"Cannot store null map document!"};
  }

  const auto id = document->get_map().get_uuid();

  mDocuments[id] = document;
  mMaps[id] = std::move(document);

  mOpenDocuments.insert(id);
  mActiveDocument = id;
}

void DocumentManager::add_tileset_document(Shared<TilesetDocument> document)
{
  if (!document) {
    throw Exception {"Cannot store null tileset document!"};
  }

  const auto id = document->get_tileset_ptr()->get_uuid();

  mDocuments[id] = document;
  mTilesets[id] = std::move(document);
}

auto DocumentManager::remove_map_document(const UUID& id) -> Shared<MapDocument>
{
  if (auto iter = mMaps.find(id); iter != mMaps.end()) {
    auto map_document = iter->second;

    mMaps.erase(iter);
    mDocuments.erase(id);
    mOpenDocuments.erase(id);

    if (mActiveDocument == id) {
      select_another_document();
    }

    remove_unused_tilesets_from(map_document->get_map().get_tileset_bundle());
    TACTILE_ASSERT(!mActiveDocument || is_document(*mActiveDocument));

    return map_document;
  }

  return nullptr;
}

auto DocumentManager::remove_tileset_document(const UUID& id) -> Shared<TilesetDocument>
{
  if (auto iter = mTilesets.find(id); iter != mTilesets.end()) {
    auto document = iter->second;

    mTilesets.erase(iter);
    mDocuments.erase(id);
    mOpenDocuments.erase(id);

    if (mActiveDocument == id) {
      select_another_document();
    }

    return document;
  }

  return nullptr;
}

void DocumentManager::select_another_document()
{
  mActiveDocument.reset();

  if (!mOpenDocuments.empty()) {
    mActiveDocument = *mOpenDocuments.begin();
  }
}

void DocumentManager::select_document(const UUID& id)
{
  if (is_document(id)) [[likely]] {
    mActiveDocument = id;
  }
  else {
    throw Exception {"Tried to select invalid document!"};
  }
}

void DocumentManager::open_document(const UUID& id)
{
  if (!exists_in(mDocuments, id)) [[unlikely]] {
    throw Exception {"Tried to open invalid document!"};
  }

  mOpenDocuments.insert(id);

  if (!mActiveDocument) {
    mActiveDocument = id;
  }
}

void DocumentManager::close_document(const UUID& id)
{
  mOpenDocuments.erase(id);

  // Maps are removed when closed, unlike tilesets
  if (exists_in(mMaps, id)) {
    remove_map_document(id);
  }

  if (mActiveDocument == id) {
    select_another_document();
  }
}

void DocumentManager::remove_unused_tilesets_from(const TilesetBundle& bundle)
{
  for (const auto& [id, tileset] : bundle) {
    if (!is_tileset_used(id)) {
      remove_tileset_document(id);
    }
  }
}

auto DocumentManager::current_document_id() const -> const Maybe<UUID>&
{
  return mActiveDocument;
}

auto DocumentManager::current_document() -> Document*
{
  return mActiveDocument ? lookup_in(mDocuments, *mActiveDocument).get() : nullptr;
}

auto DocumentManager::current_document() const -> const Document*
{
  return mActiveDocument ? lookup_in(mDocuments, *mActiveDocument).get() : nullptr;
}

auto DocumentManager::current_map_document() -> MapDocument*
{
  if (mActiveDocument) {
    if (auto iter = mMaps.find(*mActiveDocument); iter != mMaps.end()) {
      return iter->second.get();
    }
  }

  return nullptr;
}

auto DocumentManager::current_map_document() const -> const MapDocument*
{
  if (mActiveDocument) {
    if (auto iter = mMaps.find(*mActiveDocument); iter != mMaps.end()) {
      return iter->second.get();
    }
  }

  return nullptr;
}

auto DocumentManager::current_tileset_document() -> TilesetDocument*
{
  if (mActiveDocument) {
    if (auto iter = mTilesets.find(*mActiveDocument); iter != mTilesets.end()) {
      return iter->second.get();
    }
  }

  return nullptr;
}

auto DocumentManager::current_tileset_document() const -> const TilesetDocument*
{
  if (mActiveDocument) {
    if (auto iter = mTilesets.find(*mActiveDocument); iter != mTilesets.end()) {
      return iter->second.get();
    }
  }

  return nullptr;
}

auto DocumentManager::is_document_open(const UUID& id) const -> bool
{
  return mOpenDocuments.find(id) != mOpenDocuments.end();
}

auto DocumentManager::is_map_active() const -> bool
{
  return mActiveDocument && is_map(*mActiveDocument);
}

auto DocumentManager::is_tileset_active() const -> bool
{
  return mActiveDocument && is_tileset(*mActiveDocument);
}

auto DocumentManager::is_document(const UUID& id) const -> bool
{
  return exists_in(mDocuments, id);
}

auto DocumentManager::is_map(const UUID& id) const -> bool
{
  return exists_in(mMaps, id);
}

auto DocumentManager::is_tileset(const UUID& id) const -> bool
{
  return exists_in(mTilesets, id);
}

auto DocumentManager::is_tileset_used(const UUID& id) const -> bool
{
  return std::any_of(mMaps.begin(), mMaps.end(), [&](const auto& pair) {
    const auto& map = pair.second->get_map();
    return map.get_tileset_bundle().has_tileset(id);
  });
}

auto DocumentManager::has_document_with_path(const Path& path) const -> bool
{
  return std::any_of(mDocuments.begin(), mDocuments.end(), [&](const auto& pair) {
    const auto& document = pair.second;
    return document->has_path() && document->get_path() == path;
  });
}

auto DocumentManager::get_document_ptr(const UUID& id) -> Shared<Document>
{
  return lookup_in(mDocuments, id);
}

auto DocumentManager::get_map_document_ptr(const UUID& id) -> Shared<MapDocument>
{
  return lookup_in(mMaps, id);
}

auto DocumentManager::get_tileset_document_ptr(const UUID& id) -> Shared<TilesetDocument>
{
  return lookup_in(mTilesets, id);
}

auto DocumentManager::get_document(const UUID& id) const -> const Document&
{
  return *lookup_in(mDocuments, id);
}

auto DocumentManager::get_map_document(const UUID& id) const -> const MapDocument&
{
  return *lookup_in(mMaps, id);
}

auto DocumentManager::get_tileset_document(const UUID& id) const -> const TilesetDocument&
{
  return *lookup_in(mTilesets, id);
}

auto DocumentManager::first_match(const PredicateFn& pred) const -> Maybe<UUID>
{
  for (const auto& [id, document] : mDocuments) {
    if (pred(*document)) {
      return id;
    }
  }

  return nothing;
}

}  // namespace tactile
