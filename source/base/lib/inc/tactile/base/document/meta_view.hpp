// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstddef>      // size_t
#include <string>       // string
#include <string_view>  // string_view
#include <utility>      // pair

#include "tactile/base/prelude.hpp"

namespace tactile {

class Attribute;

/**
 * A read-only view of the metadata associated with a document element.
 */
class IMetaView
{
 public:
  TACTILE_INTERFACE_CLASS(IMetaView);

  /**
   * Returns the name associated with the context.
   *
   * \return
   * A context name.
   */
  [[nodiscard]]
  virtual auto get_name() const -> std::string_view = 0;

  /**
   * Returns a property attached to the context.
   *
   * \param index The property index.
   *
   * \return
   * A property name and value pair.
   */
  [[nodiscard]]
  virtual auto get_property(std::size_t index) const
      -> std::pair<const std::string&, const Attribute&> = 0;

  /**
   * Returns the number of properties attached to the context.
   *
   * \return
   * A property count.
   */
  [[nodiscard]]
  virtual auto property_count() const -> std::size_t = 0;
};

}  // namespace tactile
