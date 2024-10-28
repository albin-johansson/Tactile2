// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/model/settings.hpp"

#include <gtest/gtest.h>

#include "tactile/core/ui/i18n/language.hpp"

namespace tactile::core {
namespace {

// tactile::core::get_default_settings
TEST(Settings, GetDefaultSettings)
{
  const auto settings = get_default_settings();
  EXPECT_EQ(settings.language, ui::LanguageID::kAmericanEnglish);
  EXPECT_EQ(settings.font_size, 13.0f);
  EXPECT_EQ(settings.log_verbose_events, false);
}

}  // namespace
}  // namespace tactile::core
