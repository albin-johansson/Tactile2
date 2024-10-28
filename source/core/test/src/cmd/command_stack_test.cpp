// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/command_stack.hpp"

#include <gtest/gtest.h>

namespace tactile::core {
namespace {

struct C1 final : ICommand
{
  void undo() override
  {}

  void redo() override
  {}
};

struct C2 final : ICommand
{
  void undo() override
  {}

  void redo() override
  {}
};

// tactile::core::CommandStack::CommandStack
TEST(CommandStack, Constructor)
{
  const CommandStack command_stack {64};

  EXPECT_EQ(command_stack.size(), 0);
  EXPECT_EQ(command_stack.capacity(), 64);
  EXPECT_TRUE(command_stack.is_clean());
  EXPECT_FALSE(command_stack.can_undo());
  EXPECT_FALSE(command_stack.can_redo());
  EXPECT_FALSE(command_stack.index().has_value());
  EXPECT_FALSE(command_stack.clean_index().has_value());
}

// tactile::core::CommandStack::push
// tactile::core::CommandStack::undo
// tactile::core::CommandStack::redo
TEST(CommandStack, MixedUsage)
{
  CommandStack stack {64};
  EXPECT_EQ(stack.capacity(), 64);

  // [] -push-> [ current:C1 ]
  stack.push<C1>();
  EXPECT_EQ(stack.capacity(), 64);
  EXPECT_EQ(stack.size(), 1);
  EXPECT_EQ(stack.index(), 0);
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.can_undo());
  EXPECT_FALSE(stack.can_redo());

  // [ current:C1 ] -push-> [ C1, current:C2 ]
  stack.push<C2>();
  EXPECT_EQ(stack.capacity(), 64);
  EXPECT_EQ(stack.size(), 2);
  EXPECT_EQ(stack.index(), 1);
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.can_undo());
  EXPECT_FALSE(stack.can_redo());

  // [ C1, current:C2 ] -undo-> [ current:C1, C2 ]
  stack.undo();
  EXPECT_EQ(stack.capacity(), 64);
  EXPECT_EQ(stack.size(), 2);
  EXPECT_EQ(stack.index(), 0);
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.can_undo());
  EXPECT_TRUE(stack.can_redo());

  // [ current:C1, C2 ] -undo-> [ C1, C2 ]
  stack.undo();
  EXPECT_EQ(stack.capacity(), 64);
  EXPECT_EQ(stack.size(), 2);
  EXPECT_EQ(stack.index(), std::nullopt);
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_FALSE(stack.can_undo());
  EXPECT_TRUE(stack.can_redo());

  // [ C1, C2 ] -redo-> [ current:C1, C2 ]
  stack.redo();
  EXPECT_EQ(stack.capacity(), 64);
  EXPECT_EQ(stack.size(), 2);
  EXPECT_EQ(stack.index(), 0);
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.can_undo());
  EXPECT_TRUE(stack.can_redo());

  // [ current:C1, C2 ] -redo-> [ C1, current:C2 ]
  stack.redo();
  EXPECT_EQ(stack.capacity(), 64);
  EXPECT_EQ(stack.size(), 2);
  EXPECT_EQ(stack.index(), 1);
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.can_undo());
  EXPECT_FALSE(stack.can_redo());

  // [ C1, current:C2 ] -push-> [ C1, C2, current:C1 ]
  stack.push<C1>();
  EXPECT_EQ(stack.capacity(), 64);
  EXPECT_EQ(stack.size(), 3);
  EXPECT_EQ(stack.index(), 2);
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.can_undo());
  EXPECT_FALSE(stack.can_redo());

  // [ C1, C2, current:C1 ] -undo x3-> [ C1, C2, C1 ]
  stack.undo();
  stack.undo();
  stack.undo();
  EXPECT_EQ(stack.capacity(), 64);
  EXPECT_EQ(stack.size(), 3);
  EXPECT_EQ(stack.index(), std::nullopt);
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_FALSE(stack.can_undo());
  EXPECT_TRUE(stack.can_redo());

  // [ C1, C2, C1 ] -push-> [ current:C2 ]
  stack.push<C2>();
  EXPECT_EQ(stack.capacity(), 64);
  EXPECT_EQ(stack.size(), 1);
  EXPECT_EQ(stack.index(), 0);
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.can_undo());
  EXPECT_FALSE(stack.can_redo());
}

// tactile::core::CommandStack::mark_as_clean
// tactile::core::CommandStack::clean_index
TEST(CommandStack, CleanIndexManagement)
{
  CommandStack stack {32};

  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.is_clean());

  // This should be a no-op in this case.
  stack.mark_as_clean();
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.is_clean());

  // Adding a command should make the command stack dirty.
  stack.push<C1>();
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_FALSE(stack.is_clean());

  // No clean index is set yet, but this should make the stack clean again.
  stack.undo();
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_TRUE(stack.is_clean());

  // [] -push-> [ current:C1 ] -push-> [ C1, current:C2 ]
  stack.push<C1>();
  stack.push<C2>();
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_EQ(stack.index(), 1);
  EXPECT_FALSE(stack.is_clean());

  // [ C1, current:C2 ] -mark-as-clean-> [ C1, current|clean:C2 ]
  stack.mark_as_clean();
  EXPECT_EQ(stack.clean_index(), 1);
  EXPECT_EQ(stack.index(), 1);
  EXPECT_TRUE(stack.is_clean());

  // [ C1, current|clean:C2 ] -undo-> [ current:C1, clean:C2 ]
  stack.undo();
  EXPECT_EQ(stack.clean_index(), 1);
  EXPECT_EQ(stack.index(), 0);
  EXPECT_FALSE(stack.is_clean());

  // [ current:C1, clean:C2 ] -redo-> [ C1, current|clean:C2 ]
  stack.redo();
  EXPECT_EQ(stack.clean_index(), 1);
  EXPECT_EQ(stack.index(), 1);
  EXPECT_TRUE(stack.is_clean());

  // Special case when the clean state becomes invalidated.
  // [ C1, current|clean:C2 ] -undo-> [ current:C1, clean:C2 ] -push-> [ C1,
  // current:C1 ]
  stack.undo();
  stack.push<C1>();
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_EQ(stack.index(), 1);
  EXPECT_FALSE(stack.is_clean());

  // [ C1, current:C1 ] -mark-as-clean-> [ C1, current|clean:C1 ]
  stack.mark_as_clean();
  EXPECT_EQ(stack.clean_index(), 1);
  EXPECT_EQ(stack.index(), 1);
  EXPECT_TRUE(stack.is_clean());

  // [ C1, current|clean:C1 ] -reset-clean-index-> [ C1, current:C1 ]
  stack.reset_clean_index();
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_EQ(stack.index(), 1);
  EXPECT_FALSE(stack.is_clean());
}

// tactile::core::CommandStack::push
TEST(CommandStack, CommandOverflowWithDefinedCleanIndex)
{
  CommandStack stack {4};
  EXPECT_EQ(stack.capacity(), 4);

  // [] -push-> [ current:C1 ] -push-> [ C1, current:C2 ]
  stack.push<C1>();
  stack.push<C2>();

  // [ C1, current:C2 ] -mark-as-clean-> [ C1: current|clean:C2 ]
  stack.mark_as_clean();

  // [ C1: current|clean:C2 ] -push-> [ C1: clean:C2, current:C1 ]
  stack.push<C1>();

  // [ C1: clean:C2, current:C1 ] -push-> [ C1: clean:C2, C1, current:C2 ]
  stack.push<C2>();

  EXPECT_EQ(stack.size(), stack.capacity());
  EXPECT_EQ(stack.clean_index(), 1);
  EXPECT_EQ(stack.index(), 3);
  EXPECT_FALSE(stack.is_clean());

  // [ C1: clean:C2, C1, current:C2 ] -push-> [ clean:C2, C1, C2, current:C1 ]
  stack.push<C1>();

  EXPECT_EQ(stack.size(), stack.capacity());
  EXPECT_EQ(stack.clean_index(), 0);
  EXPECT_EQ(stack.index(), 3);
  EXPECT_FALSE(stack.is_clean());

  // [ clean:C2, C1, C2, current:C1 ] -push-> [ C1, C2, C1, current:C2 ]
  stack.push<C2>();

  EXPECT_EQ(stack.size(), stack.capacity());
  EXPECT_EQ(stack.clean_index(), std::nullopt);
  EXPECT_EQ(stack.index(), 3);
  EXPECT_FALSE(stack.is_clean());
}

// tactile::core::CommandStack::push
TEST(CommandStack, SimpleCommandOverflow)
{
  CommandStack stack {100};

  for (int index = 0; index < 101; ++index) {
    stack.push<C1>();
  }

  EXPECT_EQ(stack.size(), stack.capacity());
  EXPECT_EQ(stack.index(), stack.capacity() - 1);
}

// tactile::core::CommandStack::set_capacity
TEST(CommandStack, DynamicCapacityChange)
{
  CommandStack stack {50};

  for (int index = 0; index < 50; ++index) {
    stack.push<C1>();
  }

  EXPECT_EQ(stack.size(), stack.capacity());

  stack.set_capacity(20);
  EXPECT_EQ(stack.size(), 20);
  EXPECT_EQ(stack.capacity(), 20);

  stack.set_capacity(25);
  EXPECT_EQ(stack.size(), 20);
  EXPECT_EQ(stack.capacity(), 25);
}

}  // namespace
}  // namespace tactile::core
