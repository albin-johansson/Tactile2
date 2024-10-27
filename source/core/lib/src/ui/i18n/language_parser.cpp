// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/ui/i18n/language_parser.hpp"

#include <utility>  // move, to_underlying
#include <vector>   // vector

#include "tactile/base/container/lookup.hpp"
#include "tactile/core/debug/performance.hpp"
#include "tactile/core/io/ini.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core::ui {
namespace {

[[nodiscard]]
auto _get_misc_names() -> std::unordered_map<std::string_view, MiscLabel>
{
  return {
    {"ok", MiscLabel::kOK},
  };
}

[[nodiscard]]
auto _get_verb_names() -> std::unordered_map<std::string_view, VerbLabel>
{
  return {
    {"cancel", VerbLabel::kCancel},
    {"apply", VerbLabel::kApply},
    {"close", VerbLabel::kClose},
    {"create", VerbLabel::kCreate},
    {"rename", VerbLabel::kRename},
    {"undo", VerbLabel::kUndo},
    {"redo", VerbLabel::kRedo},
    {"save", VerbLabel::kSave},
    {"quit", VerbLabel::kQuit},
  };
}

[[nodiscard]]
auto _get_noun_names() -> std::unordered_map<std::string_view, NounLabel>
{
  return {
    {"int", NounLabel::kInt},
    {"int2", NounLabel::kInt2},
    {"int3", NounLabel::kInt3},
    {"int4", NounLabel::kInt4},
    {"float", NounLabel::kFloat},
    {"float2", NounLabel::kFloat2},
    {"float3", NounLabel::kFloat3},
    {"float4", NounLabel::kFloat4},
    {"bool", NounLabel::kBool},
    {"string", NounLabel::kString},
    {"color", NounLabel::kColor},
    {"object", NounLabel::kObject},
    {"path", NounLabel::kPath},
    {"row", NounLabel::kRow},
    {"rows", NounLabel::kRows},
    {"column", NounLabel::kColumn},
    {"columns", NounLabel::kColumns},
    {"orientation", NounLabel::kOrientation},
    {"name", NounLabel::kName},
    {"type", NounLabel::kType},
    {"value", NounLabel::kValue},
    {"tile_width", NounLabel::kTileWidth},
    {"tile_height", NounLabel::kTileHeight},
    {"light_themes", NounLabel::kLightThemes},
    {"dark_themes", NounLabel::kDarkThemes},
    {"font", NounLabel::kFont},
    {"default", NounLabel::kDefault},
    {"version", NounLabel::kVersion},
    {"project_dir", NounLabel::kProjectDir},
    {"file_menu", NounLabel::kFileMenu},
    {"edit_menu", NounLabel::kEditMenu},
    {"view_menu", NounLabel::kViewMenu},
    {"map_menu", NounLabel::kMapMenu},
    {"tileset_menu", NounLabel::kTilesetMenu},
    {"help_menu", NounLabel::kHelpMenu},
    {"debug_menu", NounLabel::kDebugMenu},
    {"recent_files_menu", NounLabel::kRecentFilesMenu},
    {"widgets_menu", NounLabel::kWidgetsMenu},
    {"theme_menu", NounLabel::kThemeMenu},
    {"export_as_menu", NounLabel::kExportAsMenu},
    {"document_dock", NounLabel::kDocumentDock},
    {"layer_dock", NounLabel::kLayerDock},
    {"tileset_dock", NounLabel::kTilesetDock},
    {"property_dock", NounLabel::kPropertyDock},
    {"component_dock", NounLabel::kComponentDock},
    {"animation_dock", NounLabel::kAnimationDock},
    {"log_dock", NounLabel::kLogDock},
    {"style_editor", NounLabel::kStyleEditorWidget},
    {"godot_export_dialog", NounLabel::kGodotExportDialog},
  };
}

[[nodiscard]]
auto _get_adjective_names() -> std::unordered_map<std::string_view, AdjectiveLabel>
{
  return {
    {"orthogonal", AdjectiveLabel::kOrthogonal},
    {"hexagonal", AdjectiveLabel::kHexagonal},
  };
}

[[nodiscard]]
auto _get_action_names() -> std::unordered_map<std::string_view, ActionLabel>
{
  return {
    {"create_map", ActionLabel::kCreateMap},
    {"create_tileset", ActionLabel::kCreateTileset},
    {"create_layer", ActionLabel::kCreateLayer},
    {"open", ActionLabel::kOpen},
    {"open_map", ActionLabel::kOpenMap},
    {"open_tileset", ActionLabel::kOpenTileset},
    {"open_component_editor", ActionLabel::kOpenComponentEditor},
    {"open_settings", ActionLabel::kOpenSettings},
    {"reset_layout", ActionLabel::kResetLayout},
    {"save_as", ActionLabel::kSaveAs},
    {"reopen_last_closed_file", ActionLabel::kReopenLastClosedFile},
    {"clear_file_history", ActionLabel::kClearFileHistory},
    {"increase_font_size", ActionLabel::kIncreaseFontSize},
    {"decrease_font_size", ActionLabel::kDecreaseFontSize},
    {"reset_font_size", ActionLabel::kResetFontSize},
    {"increase_zoom", ActionLabel::kIncreaseZoom},
    {"decrease_zoom", ActionLabel::kDecreaseZoom},
    {"reset_zoom", ActionLabel::kResetZoom},
    {"center_viewport", ActionLabel::kCenterViewport},
    {"toggle_grid", ActionLabel::kToggleGrid},
    {"pan_up", ActionLabel::kPanUp},
    {"pan_down", ActionLabel::kPanDown},
    {"pan_left", ActionLabel::kPanLeft},
    {"pan_right", ActionLabel::kPanRight},
    {"highlight_active_layer", ActionLabel::kHighlightActiveLayer},
    {"toggle_ui", ActionLabel::kToggleUi},
    {"stamp_tool", ActionLabel::kStampTool},
    {"eraser_tool", ActionLabel::kEraserTool},
    {"bucket_tool", ActionLabel::kBucketTool},
    {"object_selector_tool", ActionLabel::kObjectSelectorTool},
    {"rectangle_tool", ActionLabel::kRectangleTool},
    {"ellipse_tool", ActionLabel::kEllipseTool},
    {"point_tool", ActionLabel::kPointTool},
    {"add_tileset", ActionLabel::kAddTileset},
    {"add_row", ActionLabel::kAddRow},
    {"add_column", ActionLabel::kAddColumn},
    {"remove_row", ActionLabel::kRemoveRow},
    {"remove_column", ActionLabel::kRemoveColumn},
    {"resize", ActionLabel::kResize},
    {"create_property", ActionLabel::kCreateProperty},
    {"remove_property", ActionLabel::kRemoveProperty},
    {"rename_property", ActionLabel::kRenameProperty},
    {"change_property_type", ActionLabel::kChangePropertyType},
    {"select_image", ActionLabel::kSelectImage},
    {"fix_invalid_tiles", ActionLabel::kFixInvalidTiles},
    {"show_metadata", ActionLabel::kShowMetadata},
    {"report_bug", ActionLabel::kReportBug},
    {"about_tactile", ActionLabel::kAboutTactile},
    {"open_credits", ActionLabel::kOpenCredits},
    {"open_debugger", ActionLabel::kOpenDebugger},
    {"open_style_editor", ActionLabel::kOpenStyleEditor},
    {"open_demo_window", ActionLabel::kOpenDemoWindow},
    {"open_storage_dir", ActionLabel::kOpenStorageDir},
    {"tile_layer_item", ActionLabel::kTileLayerItem},
    {"object_layer_item", ActionLabel::kObjectLayerItem},
    {"group_layer_item", ActionLabel::kGroupLayerItem},
  };
}

[[nodiscard]]
auto _get_hint_names() -> std::unordered_map<std::string_view, HintLabel>
{
  return {
    {"context_has_no_properties", HintLabel::kContextHasNoProperties},
    {"context_has_no_components", HintLabel::kContextHasNoComponents},
    {"map_has_no_layers", HintLabel::kMapHasNoLayers},
    {"map_has_no_tilesets", HintLabel::kMapHasNoTilesets},
    {"select_tileset_image", HintLabel::kSelectTilesetImage},
  };
}

template <MapLikeType T>
void _parse_section(const StringMap<IniSection>& ini,
                    const std::string_view section_name,
                    const T& name_mapping,
                    std::vector<std::string>& strings)
{
  if (const auto* section = find_in(ini, section_name)) {
    for (const auto& [key, value] : *section) {
      if (const auto* string_id = find_in(name_mapping, key)) {
        strings[std::to_underlying(*string_id)] = value;
      }
    }
  }
}

[[nodiscard]]
auto _validate_strings(std::vector<std::string>& strings) -> std::expected<void, ErrorCode>
{
  std::size_t index {0};

  for (auto& string : strings) {
    if (string.empty()) {
      TACTILE_CORE_ERROR("std::string with ID {} is not translated", index);
      return std::unexpected {ErrorCode::kBadState};
    }

    ++index;
  }

  return {};
}

}  // namespace

LanguageParser::LanguageParser()
  : mMiscNames {_get_misc_names()},
    mVerbNames {_get_verb_names()},
    mNounNames {_get_noun_names()},
    mAdjectiveNames {_get_adjective_names()},
    mActionNames {_get_action_names()},
    mHintNames {_get_hint_names()}
{}

auto LanguageParser::parse(const LanguageID id, const std::filesystem::path& path) const
    -> std::expected<Language, ErrorCode>
{
  const ScopeProfiler profiler {"LanguageParser::parse"};

  return parse_ini(path)
      .transform([this](const IniData& ini) {
        std::vector<std::string> strings {};
        strings.resize(kLabelCount);

        _parse_section(ini, "misc", mMiscNames, strings);
        _parse_section(ini, "verb", mVerbNames, strings);
        _parse_section(ini, "noun", mNounNames, strings);
        _parse_section(ini, "adjective", mAdjectiveNames, strings);
        _parse_section(ini, "action", mActionNames, strings);
        _parse_section(ini, "hint", mHintNames, strings);

        return strings;
      })
      .and_then([id](std::vector<std::string>&& strings) {
        return _validate_strings(strings).transform(
            [id, &strings] { return Language {id, std::move(strings)}; });
      });
}

auto parse_language_from_disk(const LanguageID id) -> std::expected<Language, ErrorCode>
{
  const LanguageParser parser {};

  std::filesystem::path path {};
  switch (id) {
    case LanguageID::kAmericanEnglish: path = "assets/lang/en.ini"; break;
    case LanguageID::kBritishEnglish:  path = "assets/lang/en_GB.ini"; break;
    case LanguageID::kSwedish:         path = "assets/lang/sv.ini"; break;
    default:                           return std::unexpected {ErrorCode::kBadState};
  }

  return parser.parse(id, path);
}

}  // namespace tactile::core::ui
