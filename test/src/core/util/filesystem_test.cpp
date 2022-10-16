/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "core/util/filesystem.hpp"

#include <gtest/gtest.h>

#include "core/util/env.hpp"
#include "meta/build.hpp"

namespace tactile::test {

TEST(Filesystem, ConvertToForwardSlashes)
{
  const fs::path source = R"(C:\foo\bar\abc.yaml)";
  ASSERT_EQ("C:/foo/bar/abc.yaml", convert_to_forward_slashes(source));
}

TEST(Filesystem, HasHomePrefix)
{
  const auto home = env_var(on_windows ? "USERPROFILE" : "HOME").value();

  ASSERT_FALSE(has_home_prefix(""));
  ASSERT_FALSE(has_home_prefix("foo.cpp"));
  ASSERT_FALSE(has_home_prefix("foo/bar.yaml"));
  ASSERT_FALSE(has_home_prefix("some/random/path"));

  ASSERT_TRUE(has_home_prefix(to_path(home)));
  ASSERT_TRUE(has_home_prefix(to_path(home + "/")));
  ASSERT_TRUE(has_home_prefix(to_path(home + "/foo")));
  ASSERT_TRUE(has_home_prefix(to_path(home + "/foo.txt")));
  ASSERT_TRUE(has_home_prefix(to_path(home + "/foo/bar.txt")));
}

TEST(Filesystem, ToCanonical)
{
  const auto home = env_var(on_windows ? "USERPROFILE" : "HOME").value();

  ASSERT_EQ("~", to_canonical(to_path(home)).value());
  ASSERT_EQ("~/", to_canonical(to_path(home + '/')).value());
  ASSERT_EQ("~/foo/", to_canonical(to_path(home + "/foo/")).value());
  ASSERT_EQ("~/foo/bar.txt", to_canonical(to_path(home + "/foo/bar.txt")).value());

  ASSERT_FALSE(to_canonical("some/random/path").has_value());
  ASSERT_FALSE(to_canonical("file.txt").has_value());
}

TEST(Filesystem, ToOsString)
{
  ASSERT_FALSE(to_os_string(nullptr).has_value());

  const auto str = to_os_string("foo/bar.txt").value();

#if TACTILE_OS_WINDOWS
  ASSERT_EQ(L"foo/bar.txt", str);
#else
  ASSERT_EQ("foo/bar.txt", str);
#endif  // TACTILE_OS_WINDOWS
}

}  // namespace tactile::test
