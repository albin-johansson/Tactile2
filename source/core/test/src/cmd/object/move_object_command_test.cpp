// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/move_object_command.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "test/object_command_test.hpp"

namespace tactile::core {
namespace {

class MoveObjectCommandTest : public ObjectCommandTest
{};

// tactile::core::MoveObjectCommand::redo
// tactile::core::MoveObjectCommand::undo
TEST_F(MoveObjectCommandTest, RedoUndo)
{
  auto& registry = m_document->get_registry();
  const auto object_id = make_test_object();

  constexpr Float2 old_position {123, 456};
  constexpr Float2 new_position {987, 654};

  auto& object = registry.get<CObject>(object_id);
  object.position = old_position;

  MoveObjectCommand move_object {&m_document.value(), object_id, new_position};

  move_object.redo();
  EXPECT_EQ(object.position, new_position);

  move_object.undo();
  EXPECT_EQ(object.position, old_position);

  move_object.redo();
  EXPECT_EQ(object.position, new_position);
}

}  // namespace
}  // namespace tactile::core
