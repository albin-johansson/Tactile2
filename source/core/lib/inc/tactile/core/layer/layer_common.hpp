// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/id.hpp"
#include "tactile/base/io/save/ir.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/core/entity/entity.hpp"

namespace tactile::core {

class Registry;

/**
 * Creates a layer from an intermediate representation.
 *
 * \param registry The associated registry.
 * \param ir_layer The intermediate layer representation.
 *
 * \return
 * A layer entity.
 */
[[nodiscard]]
auto make_layer(Registry& registry, const ir::Layer& ir_layer) -> EntityID;

/**
 * Destroys a layer entity.
 *
 * \param registry     The associated registry.
 * \param layer_entity The target layer.
 *
 * \pre The specified entity must be a valid layer.
 */
void destroy_layer(Registry& registry, EntityID layer_entity);

/**
 * Creates a deep copy of a layer.
 *
 * \param[in]     registry            The associated registry.
 * \param[in]     source_layer_entity The source layer.
 * \param[in,out] next_layer_id       The next available persistent layer identifier.
 *
 * \return
 * A layer entity identifier.
 *
 * \pre The specified entity must be a valid layer.
 */
[[nodiscard]]
auto copy_layer(Registry& registry, EntityID source_layer_entity, LayerID& next_layer_id)
    -> EntityID;

}  // namespace tactile::core
