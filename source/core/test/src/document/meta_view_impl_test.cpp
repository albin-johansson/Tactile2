// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/meta_view_impl.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/map/map_spec.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {
namespace {

class MetaViewImplTest : public testing::Test
{
 public:
  MetaViewImplTest()
    : mMapSpec {TileOrientation::kOrthogonal, Extent2D {5, 5}, Int2 {32, 32}},
      mDocument {MapDocument::make(mMapSpec).value()}
  {}

 protected:
  MapSpec mMapSpec;
  MapDocument mDocument;
};

// tactile::core::MetaViewImpl::MetaViewImpl
TEST_F(MetaViewImplTest, Constructor)
{
  EXPECT_ANY_THROW(MetaViewImpl(nullptr, kInvalidEntity));
}

// tactile::core::MetaViewImpl::get_name
TEST_F(MetaViewImplTest, GetName)
{
  auto& registry = mDocument.get_registry();
  const auto map_id = registry.get<CDocumentInfo>().root;

  auto& meta = registry.get<CMeta>(map_id);
  meta.name = "foobar";

  const MetaViewImpl meta_view {&mDocument, map_id};
  EXPECT_EQ(meta_view.get_name(), "foobar");
  EXPECT_EQ(meta_view.property_count(), 0);
}

// tactile::core::MetaViewImpl::get_property
// tactile::core::MetaViewImpl::property_count
TEST_F(MetaViewImplTest, GetProperty)
{
  using testing::AnyOf;
  using testing::Eq;

  auto& registry = mDocument.get_registry();
  const auto map_id = registry.get<CDocumentInfo>().root;

  const Attribute a {1};
  const Attribute b {2};
  const Attribute c {3};

  auto& meta = registry.get<CMeta>(map_id);
  meta.properties["A"] = a;
  meta.properties["B"] = b;
  meta.properties["C"] = c;

  const MetaViewImpl meta_view {&mDocument, map_id};
  EXPECT_EQ(meta_view.get_name(), "");

  const auto property_count = meta_view.property_count();
  EXPECT_EQ(property_count, 3);

  for (std::size_t index = 0; index < property_count; ++index) {
    const auto& [name, value] = meta_view.get_property(index);

    EXPECT_THAT(name, AnyOf(Eq("A"), Eq("B"), Eq("C")));
    EXPECT_THAT(value, AnyOf(Eq(a), Eq(b), Eq(c)));
  }

  EXPECT_ANY_THROW((void) meta_view.get_property(3));
}

}  // namespace
}  // namespace tactile::core
