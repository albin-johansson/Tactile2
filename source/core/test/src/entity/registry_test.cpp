// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/entity/registry.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tactile/base/container/array.hpp"
#include "tactile/base/container/hash_map.hpp"

namespace tactile {

using testing::Const;

class RegistryTest : public testing::Test
{
 protected:
  Registry mRegistry {};
};

/// \trace tactile::Registry::make_entity
TEST_F(RegistryTest, MakeEntity)
{
  const auto a = mRegistry.make_entity();
  const auto b = mRegistry.make_entity();
  const auto c = mRegistry.make_entity();

  EXPECT_NE(a, b);
  EXPECT_NE(b, c);

  EXPECT_TRUE(mRegistry.is_valid(a));
  EXPECT_TRUE(mRegistry.is_valid(b));
  EXPECT_TRUE(mRegistry.is_valid(c));
}

/// \trace tactile::Registry::destroy
/// \trace tactile::Registry::is_valid
TEST_F(RegistryTest, Destroy)
{
  const auto entity = mRegistry.make_entity();
  EXPECT_TRUE(mRegistry.is_valid(entity));

  EXPECT_TRUE(mRegistry.destroy(entity));
  EXPECT_FALSE(mRegistry.destroy(entity));

  EXPECT_FALSE(mRegistry.is_valid(entity));
}

/// \trace tactile::Registry::add
/// \trace tactile::Registry::has
TEST_F(RegistryTest, Add)
{
  EXPECT_FALSE(mRegistry.has<int>());
  EXPECT_FALSE(mRegistry.has<float>());

  EXPECT_EQ(mRegistry.add<int>(123), 123);
  EXPECT_EQ(mRegistry.add<int>(321), 321);
  EXPECT_EQ(mRegistry.add<float>(12.3f), 12.3f);

  EXPECT_TRUE(mRegistry.has<int>());
  EXPECT_TRUE(mRegistry.has<float>());

  EXPECT_EQ(mRegistry.count<int>(), 0);
  EXPECT_EQ(mRegistry.count<float>(), 0);
  EXPECT_EQ(mRegistry.count(), 0);
}

/// \trace tactile::Registry::add [EntityID]
/// \trace tactile::Registry::has [EntityID]
TEST_F(RegistryTest, AddToEntity)
{
  const auto a = mRegistry.make_entity();
  const auto b = mRegistry.make_entity();

  EXPECT_EQ(mRegistry.add<int>(a, 42), 42);
  EXPECT_TRUE(mRegistry.has<int>(a));
  EXPECT_FALSE(mRegistry.has<int>(b));

  EXPECT_EQ(mRegistry.add<int>(a, 15), 15);
  EXPECT_TRUE(mRegistry.has<int>(a));
  EXPECT_FALSE(mRegistry.has<int>(b));
}

/// \trace tactile::Registry::erase
TEST_F(RegistryTest, Erase)
{
  Registry mRegistry {};
  const auto entity = mRegistry.make_entity();

  mRegistry.add<int>(entity, 42);
  EXPECT_TRUE(mRegistry.has<int>(entity));

  mRegistry.erase<int>(entity);
  EXPECT_FALSE(mRegistry.has<int>(entity));

  EXPECT_NO_THROW(mRegistry.erase<int>(entity));
  EXPECT_TRUE(mRegistry.is_valid(entity));
}

/// \trace tactile::Registry::detach
TEST_F(RegistryTest, Detach)
{
  const auto entity = mRegistry.make_entity();

  mRegistry.add<float>(entity, 1.5f);
  EXPECT_TRUE(mRegistry.has<float>(entity));

  EXPECT_EQ(mRegistry.detach<float>(entity), 1.5f);
  EXPECT_FALSE(mRegistry.has<float>(entity));

  EXPECT_EQ(mRegistry.detach<float>(entity), nothing);
}

/// \trace tactile::Registry::get
TEST_F(RegistryTest, Get)
{
  mRegistry.add<int>(0xDEAD);

  EXPECT_TRUE(mRegistry.has<int>());
  EXPECT_EQ(mRegistry.get<int>(), 0xDEAD);
  EXPECT_EQ(Const(mRegistry).get<int>(), 0xDEAD);

  EXPECT_THROW((void) mRegistry.get<double>(), Exception);
  EXPECT_THROW((void) Const(mRegistry).get<double>(), Exception);
}

/// \trace tactile::Registry::get [EntityID]
TEST_F(RegistryTest, GetFromEntity)
{
  const auto a = mRegistry.make_entity();
  const auto b = mRegistry.make_entity();

  mRegistry.add<int>(a, 10);
  mRegistry.add<int>(b, 20);

  EXPECT_EQ(mRegistry.get<int>(a), 10);
  EXPECT_EQ(mRegistry.get<int>(b), 20);
  EXPECT_EQ(Const(mRegistry).get<int>(a), 10);
  EXPECT_EQ(Const(mRegistry).get<int>(b), 20);

  EXPECT_THROW((void) mRegistry.get<float>(a), Exception);
  EXPECT_THROW((void) Const(mRegistry).get<float>(b), Exception);
}

/// \trace tactile::Registry::find
TEST_F(RegistryTest, Find)
{
  mRegistry.add<uint32>(0xBEEFu);

  EXPECT_EQ(mRegistry.find<uint32>(), &mRegistry.get<uint32>());
  EXPECT_EQ(Const(mRegistry).find<uint32>(), &Const(mRegistry).get<uint32>());

  EXPECT_EQ(mRegistry.find<int32>(), nullptr);
  EXPECT_EQ(Const(mRegistry).find<int32>(), nullptr);
}

/// \trace tactile::Registry::find [EntityID]
TEST_F(RegistryTest, FindInEntity)
{
  const auto a = mRegistry.make_entity();
  const auto b = mRegistry.make_entity();

  mRegistry.add<float>(a, 5.0f);
  mRegistry.add<float>(b, 8.0f);

  EXPECT_EQ(mRegistry.find<float>(a), &mRegistry.get<float>(a));
  EXPECT_EQ(mRegistry.find<float>(b), &mRegistry.get<float>(b));
  EXPECT_EQ(Const(mRegistry).find<float>(a), &Const(mRegistry).get<float>(a));
  EXPECT_EQ(Const(mRegistry).find<float>(b), &Const(mRegistry).get<float>(b));

  EXPECT_EQ(mRegistry.find<bool>(kInvalidEntity), nullptr);
  EXPECT_EQ(mRegistry.find<bool>(a), nullptr);
  EXPECT_EQ(Const(mRegistry).find<bool>(kInvalidEntity), nullptr);
  EXPECT_EQ(Const(mRegistry).find<bool>(b), nullptr);
}

/// \trace tactile::Registry::each
TEST_F(RegistryTest, Each)
{
  const Array<EntityID, 4> entities = {
    mRegistry.make_entity(),
    mRegistry.make_entity(),
    mRegistry.make_entity(),
    mRegistry.make_entity(),
  };

  mRegistry.add<int>(entities[0], 1);
  mRegistry.add<int>(entities[1], 2);
  mRegistry.add<int>(entities[2], 3);
  mRegistry.add<float>(entities[3], 4.0f);

  HashMap<EntityID, int> observed_ints {};

  for (auto [entity, value] : mRegistry.each<int>()) {
    observed_ints[entity] = value;
  }

  ASSERT_TRUE(observed_ints.contains(entities[0]));
  ASSERT_TRUE(observed_ints.contains(entities[1]));
  ASSERT_TRUE(observed_ints.contains(entities[2]));
  ASSERT_FALSE(observed_ints.contains(entities[3]));
  EXPECT_EQ(observed_ints.at(entities[0]), 1);
  EXPECT_EQ(observed_ints.at(entities[1]), 2);
  EXPECT_EQ(observed_ints.at(entities[2]), 3);

  observed_ints.clear();

  for (auto [entity, value] : Const(mRegistry).each<int>()) {
    observed_ints[entity] = value;
  }

  ASSERT_TRUE(observed_ints.contains(entities[0]));
  ASSERT_TRUE(observed_ints.contains(entities[1]));
  ASSERT_TRUE(observed_ints.contains(entities[2]));
  ASSERT_FALSE(observed_ints.contains(entities[3]));
  EXPECT_EQ(observed_ints.at(entities[0]), 1);
  EXPECT_EQ(observed_ints.at(entities[1]), 2);
  EXPECT_EQ(observed_ints.at(entities[2]), 3);
}

/// \trace tactile::Registry::count [T]
TEST_F(RegistryTest, CountForSpecificComponent)
{
  EXPECT_EQ(mRegistry.count<int>(), 0);

  mRegistry.add<int>(mRegistry.make_entity());
  mRegistry.add<int>(mRegistry.make_entity());

  EXPECT_EQ(mRegistry.count<int>(), 2);
  EXPECT_EQ(mRegistry.count<float>(), 0);
}

/// \trace tactile::Registry::count
TEST_F(RegistryTest, Count)
{
  EXPECT_EQ(mRegistry.count(), 0);

  const auto a = mRegistry.make_entity();
  const auto b = mRegistry.make_entity();
  const auto c = mRegistry.make_entity();
  const auto d = mRegistry.make_entity();

  mRegistry.add<int>(a);
  mRegistry.add<int>(b);
  mRegistry.add<int>(c);
  mRegistry.add<int>(d);
  mRegistry.add<float>(a);
  mRegistry.add<float>(b);
  mRegistry.add<bool>(c);

  EXPECT_EQ(mRegistry.count(), 7);
}

}  // namespace tactile
