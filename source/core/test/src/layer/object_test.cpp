// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/layer/object.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/meta/meta.hpp"
#include "tactile/core/test/ir_comparison.hpp"
#include "tactile/test_util/ir_presets.hpp"

namespace tactile::core {
namespace {

class ObjectTest : public testing::Test
{
 protected:
  Registry mRegistry {};
};

// tactile::core::is_object
TEST_F(ObjectTest, IsObject)
{
  EXPECT_FALSE(is_object(mRegistry, kInvalidEntity));
  EXPECT_FALSE(is_object(mRegistry, mRegistry.make_entity()));

  const auto entity = mRegistry.make_entity();
  mRegistry.add<CMeta>(entity);
  mRegistry.add<CObject>(entity);

  EXPECT_TRUE(is_object(mRegistry, entity));
}

// tactile::core::make_object [Registry&, ObjectID, ObjectType]
TEST_F(ObjectTest, MakeObject)
{
  const ObjectID object_id {42};
  const auto object_type = ObjectType::kRect;
  const auto object_entity = make_object(mRegistry, object_id, object_type);

  ASSERT_TRUE(is_object(mRegistry, object_entity));
  ASSERT_TRUE(mRegistry.has<CMeta>(object_entity));
  ASSERT_TRUE(mRegistry.has<CObject>(object_entity));

  const auto& meta = mRegistry.get<CMeta>(object_entity);
  const auto& object = mRegistry.get<CObject>(object_entity);

  EXPECT_EQ(meta.name, "");
  EXPECT_EQ(meta.properties.size(), 0);

  EXPECT_EQ(object.id, object_id);
  EXPECT_EQ(object.type, object_type);
  EXPECT_EQ(object.position.x(), 0.0f);
  EXPECT_EQ(object.position.y(), 0.0f);
  EXPECT_EQ(object.size.x(), 0.0f);
  EXPECT_EQ(object.size.y(), 0.0f);
  EXPECT_EQ(object.tag, "");
  EXPECT_EQ(object.is_visible, true);
}

// tactile::core::make_object [Registry&, const ir::Object&]
TEST_F(ObjectTest, MakeObjectFromIR)
{
  const auto ir_object = test::make_complex_ir_object(ObjectID {42}, ObjectType::kEllipse);

  const auto object_id = make_object(mRegistry, ir_object);
  ASSERT_TRUE(is_object(mRegistry, object_id));

  compare_object(mRegistry, object_id, ir_object);
}

// tactile::core::copy_object
TEST_F(ObjectTest, CopyObject)
{
  const auto e1 = make_object(mRegistry, ObjectID {1}, ObjectType::kEllipse);

  auto& meta1 = mRegistry.get<CMeta>(e1);
  meta1.name = "foobar";
  meta1.properties["1"] = Attribute {123};
  meta1.properties["2"] = Attribute {"deadbeef"};

  auto& object1 = mRegistry.get<CObject>(e1);
  object1.tag = "tag";
  object1.position = Float2 {12, 34};
  object1.size = Float2 {56, 78};
  object1.is_visible = true;

  const auto e2 = copy_object(mRegistry, e1);
  EXPECT_TRUE(is_object(mRegistry, e1));
  EXPECT_TRUE(is_object(mRegistry, e2));

  const auto& meta2 = mRegistry.get<CMeta>(e2);
  EXPECT_EQ(meta1.name, meta2.name);
  EXPECT_EQ(meta1.properties, meta2.properties);
  EXPECT_EQ(meta1.components, meta2.components);

  const auto& object2 = mRegistry.get<CObject>(e2);
  EXPECT_EQ(object1.id, object2.id);
  EXPECT_EQ(object1.position, object2.position);
  EXPECT_EQ(object1.size, object2.size);
  EXPECT_EQ(object1.type, object2.type);
  EXPECT_EQ(object1.tag, object2.tag);
  EXPECT_EQ(object1.is_visible, object2.is_visible);
}

}  // namespace
}  // namespace tactile::core
