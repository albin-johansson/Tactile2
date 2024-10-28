// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/event_dispatcher.hpp"

#include <vector>  // vector

#include <gtest/gtest.h>

namespace tactile::core {
namespace {

class EventDispatcherTest : public testing::Test
{
 public:
  void on_int(const int value)
  {
    mInts.push_back(value);
  }

  void on_float(const float value)
  {
    mFloats.push_back(value);
  }

 protected:
  EventDispatcher mDispatcher {};
  std::vector<int> mInts {};
  std::vector<float> mFloats {};
};

// tactile::core::EventDispatcher::push
// tactile::core::EventDispatcher::update
// tactile::core::EventDispatcher::bind
TEST_F(EventDispatcherTest, PushAndUpdate)
{
  mDispatcher.bind<int, &EventDispatcherTest::on_int>(this);
  mDispatcher.bind<float, &EventDispatcherTest::on_float>(this);

  ASSERT_EQ(mInts.size(), 0);
  ASSERT_EQ(mFloats.size(), 0);

  mDispatcher.push<int>(1);
  mDispatcher.push<int>(2);
  mDispatcher.push<int>(3);
  mDispatcher.push<float>(1.5f);

  EXPECT_EQ(mInts.size(), 0);
  EXPECT_EQ(mFloats.size(), 0);

  mDispatcher.update();

  ASSERT_EQ(mInts.size(), 3);
  ASSERT_EQ(mFloats.size(), 1);
  EXPECT_EQ(mInts.at(0), 1);
  EXPECT_EQ(mInts.at(1), 2);
  EXPECT_EQ(mInts.at(2), 3);
  EXPECT_EQ(mFloats.at(0), 1.5f);

  mDispatcher.update();

  EXPECT_EQ(mInts.size(), 3);
  EXPECT_EQ(mFloats.size(), 1);
}

// tactile::core::EventDispatcher::push
// tactile::core::EventDispatcher::update
// tactile::core::EventDispatcher::bind
TEST_F(EventDispatcherTest, PushAndUpdateUnregisteredEvent)
{
  ASSERT_EQ(mInts.size(), 0);
  ASSERT_EQ(mFloats.size(), 0);

  EXPECT_NO_FATAL_FAILURE(mDispatcher.push<double>(1.0));
  EXPECT_NO_FATAL_FAILURE(mDispatcher.update());

  EXPECT_EQ(mInts.size(), 0);
  EXPECT_EQ(mFloats.size(), 0);
}

// tactile::core::EventDispatcher::trigger
// tactile::core::EventDispatcher::bind
TEST_F(EventDispatcherTest, Trigger)
{
  mDispatcher.bind<int, &EventDispatcherTest::on_int>(this);

  ASSERT_EQ(mInts.size(), 0);
  ASSERT_EQ(mFloats.size(), 0);

  mDispatcher.trigger<int>(42);

  ASSERT_EQ(mInts.size(), 1);
  ASSERT_EQ(mFloats.size(), 0);
  EXPECT_EQ(mInts.at(0), 42);

  EXPECT_NO_FATAL_FAILURE(mDispatcher.trigger<float>(1.23f));
  EXPECT_EQ(mInts.size(), 1);
  EXPECT_EQ(mFloats.size(), 0);
}

// tactile::core::EventDispatcher::trigger
TEST_F(EventDispatcherTest, TriggerUnregisteredEvent)
{
  ASSERT_EQ(mInts.size(), 0);
  ASSERT_EQ(mFloats.size(), 0);

  EXPECT_NO_FATAL_FAILURE(mDispatcher.trigger<int>(100));

  EXPECT_EQ(mInts.size(), 0);
  EXPECT_EQ(mFloats.size(), 0);
}

}  // namespace
}  // namespace tactile::core
