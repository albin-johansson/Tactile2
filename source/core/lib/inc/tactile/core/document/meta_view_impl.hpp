// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/document/document.hpp"
#include "tactile/base/document/meta_view.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/core/entity/entity.hpp"

namespace tactile::core {

/**
 * A metadata view implementation.
 */
class MetaViewImpl final : public IMetaView
{
 public:
  /**
   * Creates a view.
   *
   * \param document The associated document.
   * \param meta_id  The metadata context entity identifier.
   */
  MetaViewImpl(const IDocument* document, EntityID meta_id);

  [[nodiscard]]
  auto get_name() const -> std::string_view override;

  [[nodiscard]]
  auto get_property(std::size_t index) const
      -> std::pair<const std::string&, const Attribute&> override;

  [[nodiscard]]
  auto property_count() const -> std::size_t override;

 private:
  const IDocument* mDocument;
  EntityID mMetaId;
};

}  // namespace tactile::core
