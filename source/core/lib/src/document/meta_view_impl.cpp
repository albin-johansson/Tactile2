// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/meta_view_impl.hpp"

#include <iterator>   // next
#include <stdexcept>  // out_of_range

#include "tactile/base/debug/validation.hpp"
#include "tactile/base/document/document.hpp"
#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {

MetaViewImpl::MetaViewImpl(const IDocument* document, const EntityID meta_id)
  : mDocument {require_not_null(document, "null document")},
    mMetaId {meta_id}
{}

auto MetaViewImpl::get_name() const -> std::string_view
{
  const auto& registry = mDocument->get_registry();
  const auto& meta = registry.get<CMeta>(mMetaId);

  return meta.name;
}

auto MetaViewImpl::get_property(const std::size_t index) const
    -> std::pair<const std::string&, const Attribute&>
{
  const auto& registry = mDocument->get_registry();
  const auto& meta = registry.get<CMeta>(mMetaId);

  if (index >= meta.properties.size()) {
    throw std::out_of_range {"bad property index"};
  }

  const auto iter = std::next(meta.properties.begin(), saturate_cast<std::ptrdiff_t>(index));
  return {iter->first, iter->second};
}

auto MetaViewImpl::property_count() const -> std::size_t
{
  const auto& registry = mDocument->get_registry();
  const auto& meta = registry.get<CMeta>(mMetaId);

  return meta.properties.size();
}

}  // namespace tactile::core
