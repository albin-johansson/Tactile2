// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/platform/filesystem.hpp"

#include <gtest/gtest.h>

#include "tactile/core/platform/environment.hpp"

namespace tactile::core {
namespace {

// tactile::core::get_persistent_storage_directory
TEST(Filesystem, GetPersistentStorageDirectory)
{
  const auto dir = get_persistent_storage_directory();
  EXPECT_TRUE(dir.has_value());
}

// tactile::core::get_user_home_directory
TEST(Filesystem, GetUserHomeDirectory)
{
  const std::filesystem::path home_path {get_env(kOnWindows ? "USERPROFILE" : "HOME").value()};
  const auto dir = get_user_home_directory();
  EXPECT_EQ(dir, home_path);
}

// tactile::core::normalize_path
TEST(Filesystem, NormalizePath)
{
  EXPECT_EQ(normalize_path(R"(~/a/b)"), "~/a/b");
  EXPECT_EQ(normalize_path(R"(C:\a\b\c.txt)"), "C:/a/b/c.txt");
}

// tactile::core::strip_home_directory_prefix
TEST(Filesystem, StripHomeDirectoryPrefix)
{
  const auto home_dir = get_user_home_directory().value();

  const std::string abc {"/a/b/c"};
  const std::string foobar {"/foobar"};

  EXPECT_EQ(strip_home_directory_prefix(home_dir + abc, home_dir), "~" + abc);
  EXPECT_EQ(strip_home_directory_prefix(home_dir + foobar, home_dir), "~" + foobar);

  EXPECT_FALSE(strip_home_directory_prefix(abc, home_dir).has_value());
}

}  // namespace
}  // namespace tactile::core
