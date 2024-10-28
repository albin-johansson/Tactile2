// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/set_object_tag_command.hpp"

#include <string>  // string

#include <gtest/gtest.h>

#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "test/object_command_test.hpp"

namespace tactile::core {
namespace {

class SetObjectTagCommandTest : public ObjectCommandTest
{};

// tactile::core::SetObjectTagCommand::redo
// tactile::core::SetObjectTagCommand::undo
TEST_F(SetObjectTagCommandTest, RedoUndo)
{
  const std::string old_tag {"foo"};
  const std::string new_tag {"bar"};

  const auto object_id = make_test_object();

  auto& registry = m_document->get_registry();
  auto& object = registry.get<CObject>(object_id);
  object.tag = old_tag;

  SetObjectTagCommand set_object_tag {&m_document.value(), object_id, new_tag};

  set_object_tag.redo();
  EXPECT_EQ(object.tag, new_tag);

  set_object_tag.undo();
  EXPECT_EQ(object.tag, old_tag);

  set_object_tag.redo();
  EXPECT_EQ(object.tag, new_tag);
}

// tactile::core::SetObjectTagCommand::merge_with
TEST_F(SetObjectTagCommandTest, MergeWith)
{
  const std::string tag0 {"0"};
  const std::string tag1 {"1"};
  const std::string tag2 {"2"};
  const std::string tag3 {"3"};

  const auto object_id = make_test_object();

  auto& registry = m_document->get_registry();
  auto& object = registry.get<CObject>(object_id);
  object.tag = tag0;

  SetObjectTagCommand set_object_tag1 {&m_document.value(), object_id, tag1};
  const SetObjectTagCommand set_object_tag2 {&m_document.value(), object_id, tag2};
  const SetObjectTagCommand set_object_tag3 {&m_document.value(), object_id, tag3};

  EXPECT_TRUE(set_object_tag1.merge_with(&set_object_tag2));
  EXPECT_TRUE(set_object_tag1.merge_with(&set_object_tag3));

  set_object_tag1.redo();
  EXPECT_EQ(object.tag, tag3);

  set_object_tag1.undo();
  EXPECT_EQ(object.tag, tag0);
}

}  // namespace
}  // namespace tactile::core
