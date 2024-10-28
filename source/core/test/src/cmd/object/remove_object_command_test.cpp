// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/remove_object_command.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/layer/object.hpp"
#include "test/object_command_test.hpp"

namespace tactile::core {
namespace {

class RemoveObjectCommandTest : public ObjectCommandTest
{
 public:
  void SetUp() override
  {
    ObjectCommandTest::SetUp();
    m_object_id = make_test_object();
  }

 protected:
  EntityID m_object_id {kInvalidEntity};
};

// tactile::core::RemoveObjectCommand::redo
// tactile::core::RemoveObjectCommand::undo
TEST_F(RemoveObjectCommandTest, RedoUndo)
{
  using testing::Contains;
  using testing::Not;

  auto& registry = m_document->get_registry();
  const auto& object_layer = registry.get<CObjectLayer>(m_layer_id);

  ASSERT_TRUE(is_object(registry, m_object_id));
  ASSERT_THAT(object_layer.objects, Contains(m_object_id));

  RemoveObjectCommand remove_object {&m_document.value(), m_layer_id, m_object_id};
  remove_object.redo();

  EXPECT_TRUE(is_object(registry, m_object_id));
  EXPECT_THAT(object_layer.objects, Not(Contains(m_object_id)));

  remove_object.undo();

  EXPECT_TRUE(is_object(registry, m_object_id));
  EXPECT_THAT(object_layer.objects, Contains(m_object_id));

  remove_object.redo();

  EXPECT_TRUE(is_object(registry, m_object_id));
  EXPECT_THAT(object_layer.objects, Not(Contains(m_object_id)));
}

// tactile::core::RemoveObjectCommand::dispose
TEST_F(RemoveObjectCommandTest, Dispose)
{
  const auto& registry = m_document->get_registry();

  RemoveObjectCommand remove_object {&m_document.value(), m_layer_id, m_object_id};
  remove_object.redo();
  remove_object.undo();

  remove_object.dispose();
  EXPECT_TRUE(registry.is_valid(m_object_id));
  EXPECT_TRUE(is_object(registry, m_object_id));

  remove_object.redo();
  EXPECT_TRUE(registry.is_valid(m_object_id));
  EXPECT_TRUE(is_object(registry, m_object_id));

  remove_object.dispose();
  EXPECT_FALSE(registry.is_valid(m_object_id));
  EXPECT_FALSE(is_object(registry, m_object_id));
}

}  // namespace
}  // namespace tactile::core
