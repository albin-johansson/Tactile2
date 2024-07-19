// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/meta/meta.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tactile/base/test_util/ir_presets.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/test/ir_comparison.hpp"

namespace tactile::test {

// tactile::is_meta
TEST(Meta, IsMeta)
{
  Registry registry {};

  const auto meta_id = registry.make_entity();
  registry.add<CMeta>(meta_id);

  EXPECT_FALSE(is_meta(registry, kInvalidEntity));
  EXPECT_FALSE(is_meta(registry, registry.make_entity()));
  EXPECT_TRUE(is_meta(registry, meta_id));
}

// tactile::convert_ir_metadata
TEST(Meta, ConvertIrMetadata)
{
  Registry registry {};

  const auto meta_id = registry.make_entity();
  registry.add<CMeta>(meta_id);

  const auto ir_metadata = make_complex_ir_metadata("Meta");
  convert_ir_metadata(registry, meta_id, ir_metadata);

  compare_meta(registry, meta_id, ir_metadata);
}

}  // namespace tactile::test
