// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/object_view_impl.hpp"

#include "tactile/base/document/document.hpp"
#include "tactile/base/document/document_visitor.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"

namespace tactile::core {

ObjectViewImpl::ObjectViewImpl(const IDocument* document,
                               const ILayerView* parent_layer,
                               const EntityID object_id)
  : mDocument {require_not_null(document, "null document")},
    mParentLayer {require_not_null(parent_layer, "null layer view")},
    mParentTile {nullptr},
    mObjectId {object_id},
    mMeta {document, object_id}
{}

ObjectViewImpl::ObjectViewImpl(const IDocument* document,
                               const ITileView* parent_tile,
                               const EntityID object_id)
  : mDocument {require_not_null(document, "null document")},
    mParentLayer {nullptr},
    mParentTile {require_not_null(parent_tile, "null tile view")},
    mObjectId {object_id},
    mMeta {document, object_id}
{}

auto ObjectViewImpl::accept(IDocumentVisitor& visitor) const -> std::expected<void, ErrorCode>
{
  return visitor.visit(*this);
}

auto ObjectViewImpl::get_parent_layer() const -> const ILayerView*
{
  return mParentLayer;
}

auto ObjectViewImpl::get_parent_tile() const -> const ITileView*
{
  return mParentTile;
}

auto ObjectViewImpl::get_type() const -> ObjectType
{
  const auto& registry = mDocument->get_registry();
  const auto& object = registry.get<CObject>(mObjectId);

  return object.type;
}

auto ObjectViewImpl::get_id() const -> ObjectID
{
  const auto& registry = mDocument->get_registry();
  const auto& object = registry.get<CObject>(mObjectId);

  return object.id;
}

auto ObjectViewImpl::get_position() const -> Float2
{
  const auto& registry = mDocument->get_registry();
  const auto& object = registry.get<CObject>(mObjectId);

  return object.position;
}

auto ObjectViewImpl::get_size() const -> Float2
{
  const auto& registry = mDocument->get_registry();
  const auto& object = registry.get<CObject>(mObjectId);

  return object.size;
}

auto ObjectViewImpl::get_tag() const -> std::string_view
{
  const auto& registry = mDocument->get_registry();
  const auto& object = registry.get<CObject>(mObjectId);

  return object.tag;
}

auto ObjectViewImpl::is_visible() const -> bool
{
  const auto& registry = mDocument->get_registry();
  const auto& object = registry.get<CObject>(mObjectId);

  return object.is_visible;
}

auto ObjectViewImpl::get_meta() const -> const IMetaView&
{
  return mMeta;
}

}  // namespace tactile::core
