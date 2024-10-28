// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/create_object_command.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/layer/object.hpp"
#include "tactile/core/map/map.hpp"
#include "test/object_command_test.hpp"

namespace tactile::core {
namespace {

class CreateObjectCommandTest : public ObjectCommandTest
{};

// tactile::core::CreateObjectCommand::redo
// tactile::core::CreateObjectCommand::undo
TEST_F(CreateObjectCommandTest, RedoUndo)
{
  using testing::Contains;
  using testing::Not;

  auto& registry = m_document->get_registry();

  const auto& id_cache = registry.get<CMapIdCache>(m_map_id);
  const auto& object_layer = registry.get<CObjectLayer>(m_layer_id);

  constexpr auto object_type = ObjectType::kEllipse;
  constexpr Float2 object_pos {12, 34};
  constexpr Float2 object_size {56, 78};

  CreateObjectCommand create_object {&m_document.value(),
                                     m_layer_id,
                                     object_type,
                                     object_pos,
                                     object_size};
  create_object.redo();

  ASSERT_EQ(object_layer.objects.size(), 1);

  const auto object_id = object_layer.objects.front();
  ASSERT_TRUE(is_object(registry, object_id));

  {
    const auto& object = registry.get<CObject>(object_id);
    EXPECT_EQ(object.id, id_cache.next_object_id - 1);
    EXPECT_EQ(object.type, object_type);
    EXPECT_EQ(object.position, object_pos);
    EXPECT_EQ(object.size, object_size);
    EXPECT_EQ(object.tag, "");
    EXPECT_TRUE(object.is_visible);
  }

  create_object.undo();

  EXPECT_TRUE(is_object(registry, object_id));
  EXPECT_THAT(object_layer.objects, Not(Contains(object_id)));

  create_object.redo();

  EXPECT_TRUE(is_object(registry, object_id));
  EXPECT_THAT(object_layer.objects, Contains(object_id));
}

// tactile::core::CreateObjectCommand::dispose
TEST_F(CreateObjectCommandTest, Dispose)
{
  auto& registry = m_document->get_registry();

  const auto& object_layer = registry.get<CObjectLayer>(m_layer_id);

  constexpr auto object_type = ObjectType::kRect;
  constexpr Float2 object_pos {10, 20};
  constexpr Float2 object_size {30, 40};

  CreateObjectCommand create_object {&m_document.value(),
                                     m_layer_id,
                                     object_type,
                                     object_pos,
                                     object_size};
  create_object.redo();

  const auto object_id = object_layer.objects.at(0);
  ASSERT_TRUE(is_object(registry, object_id));

  create_object.dispose();
  EXPECT_TRUE(registry.is_valid(object_id));
  EXPECT_TRUE(is_object(registry, object_id));

  create_object.undo();
  EXPECT_TRUE(registry.is_valid(object_id));
  EXPECT_TRUE(is_object(registry, object_id));

  create_object.dispose();
  EXPECT_FALSE(registry.is_valid(object_id));
  EXPECT_FALSE(is_object(registry, object_id));
}

}  // namespace
}  // namespace tactile::core
