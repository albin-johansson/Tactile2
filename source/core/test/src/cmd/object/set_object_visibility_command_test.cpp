// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/set_object_visibility_command.hpp"

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "test/object_command_test.hpp"

namespace tactile::core {
namespace {

class SetObjectVisibilityCommandTest : public ObjectCommandTest
{};

// tactile::core::SetObjectVisibilityCommand::redo
// tactile::core::SetObjectVisibilityCommand::undo
TEST_F(SetObjectVisibilityCommandTest, RedoUndo)
{
  const auto object_id = make_test_object();

  constexpr bool old_visibility {false};
  constexpr bool new_visibility {true};

  auto& registry = m_document->get_registry();
  auto& object = registry.get<CObject>(object_id);
  object.is_visible = old_visibility;

  SetObjectVisibilityCommand set_visibility {&m_document.value(), object_id, new_visibility};

  set_visibility.redo();
  EXPECT_EQ(object.is_visible, new_visibility);

  set_visibility.undo();
  EXPECT_EQ(object.is_visible, old_visibility);

  set_visibility.redo();
  EXPECT_EQ(object.is_visible, new_visibility);
}

}  // namespace
}  // namespace tactile::core
