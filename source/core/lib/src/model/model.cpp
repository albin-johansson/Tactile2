// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/model/model.hpp"

#include "tactile/base/debug/validation.hpp"
#include "tactile/core/model/settings.hpp"

namespace tactile::core {

Model::Model(Settings* settings, const ui::Language* language)
  : mSettings {require_not_null(settings, "null settings")},
    mLanguage {require_not_null(language, "null language")}
{
  mDocuments.set_command_capacity(mSettings->command_capacity);
}

auto Model::get_document_manager() -> DocumentManager&
{
  return mDocuments;
}

auto Model::get_document_manager() const -> const DocumentManager&
{
  return mDocuments;
}

auto Model::get_current_document() -> IDocument*
{
  return mDocuments.get_current_document();
}

auto Model::get_current_document() const -> const IDocument*
{
  return mDocuments.get_current_document();
}

auto Model::get_settings() -> Settings&
{
  return *mSettings;
}

auto Model::get_settings() const -> const Settings&
{
  return *mSettings;
}

auto Model::get_language() const -> const ui::Language&
{
  return *mLanguage;
}

}  // namespace tactile::core
