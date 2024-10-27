// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/ui/menu_bar.hpp"

#include <SDL2/SDL.h>
#include <imgui.h>

#include "tactile/core/document/document_info.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/platform/filesystem.hpp"
#include "tactile/core/ui/common/menus.hpp"
#include "tactile/core/ui/common/widgets.hpp"
#include "tactile/core/ui/common/window.hpp"
#include "tactile/core/ui/i18n/language.hpp"
#include "tactile/core/ui/shortcuts.hpp"

namespace tactile::core::ui {
namespace {

void _push_recent_files_menu(const Language& language, EventDispatcher& dispatcher)
{
  if (const MenuScope menu {language.get(NounLabel::kRecentFilesMenu)}; menu.is_open()) {
    if (ImGui::MenuItem(language.get(ActionLabel::kReopenLastClosedFile),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<ReopenLastClosedFileEvent>();
    }

    ImGui::Separator();

    // TODO iterate recent files

    if (ImGui::MenuItem(language.get(ActionLabel::kClearFileHistory), nullptr, false, false)) {
      dispatcher.push<ClearFileHistoryEvent>();
    }
  }
}

void _push_file_menu(const Model& model, EventDispatcher& dispatcher)
{
  const auto& language = model.get_language();
  const auto* current_document = model.get_current_document();

  if (const MenuScope menu {language.get(NounLabel::kFileMenu)}; menu.is_open()) {
    if (ImGui::MenuItem(language.get(ActionLabel::kCreateMap), kCreateMapShortcut.hint)) {
      dispatcher.push<ShowNewMapDialogEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kOpen), kOpenShortcut.hint)) {
      dispatcher.push<ShowOpenMapDialogEvent>();
    }

    _push_recent_files_menu(language, dispatcher);

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(VerbLabel::kSave), kSaveShortcut.hint, false, false)) {
      dispatcher.push<SaveEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kSaveAs),
                        kSaveAsShortcut.hint,
                        false,
                        current_document != nullptr)) {
      dispatcher.push<ShowSaveAsDialogEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(VerbLabel::kClose),
                        nullptr,
                        false,
                        current_document != nullptr)) {
      dispatcher.push<CloseEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(VerbLabel::kQuit))) {
      dispatcher.push<QuitEvent>();
    }
  }
}

void _push_edit_menu(const Model& model, EventDispatcher& dispatcher)
{
  const auto& language = model.get_language();
  const auto& document_manager = model.get_document_manager();

  const auto* document = model.get_current_document();
  const auto* history =
      (document != nullptr) ? &document_manager.get_history(document->get_uuid()) : nullptr;

  if (const MenuScope menu {language.get(NounLabel::kEditMenu)}; menu.is_open()) {
    if (ImGui::MenuItem(language.get(VerbLabel::kUndo),
                        kUndoShortcut.hint,
                        false,
                        (history != nullptr) && history->can_undo())) {
      dispatcher.push<UndoEvent>();
    }

    if (ImGui::MenuItem(language.get(VerbLabel::kRedo),
                        kRedoShortcut.hint,
                        false,
                        (history != nullptr) && history->can_redo())) {
      dispatcher.push<RedoEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kStampTool), nullptr, false, false)) {
      dispatcher.push<EnableStampToolEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kEraserTool), nullptr, false, false)) {
      dispatcher.push<EnableEraserToolEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kBucketTool), nullptr, false, false)) {
      dispatcher.push<EnableBucketToolEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kObjectSelectorTool),
                        nullptr,
                        false,
                        false)) {
      dispatcher.push<EnableSelectorToolEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kRectangleTool), nullptr, false, false)) {
      dispatcher.push<EnableRectangleToolEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kEllipseTool), nullptr, false, false)) {
      dispatcher.push<EnableEllipseToolEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kPointTool), nullptr, false, false)) {
      dispatcher.push<EnablePointToolEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kOpenComponentEditor),
                        kOpenComponentEditorShortcut.hint,
                        false,
                        document != nullptr)) {
      dispatcher.push<ShowComponentEditorDialogEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kOpenSettings),
                        kOpenSettingsShortcut.hint)) {
      dispatcher.push<ShowSettingsDialogEvent>();
    }
  }
}

void _push_widgets_menu(const Language& language, EventDispatcher& dispatcher)
{
  if (const MenuScope widgets_menu {language.get(NounLabel::kWidgetsMenu)};
      widgets_menu.is_open()) {
    if (ImGui::MenuItem(language.get(ActionLabel::kResetLayout))) {
      dispatcher.push<ResetLayoutEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(NounLabel::kPropertyDock), nullptr, true)) {
      dispatcher.push<TogglePropertyDockEvent>();
    }

    if (ImGui::MenuItem(language.get(NounLabel::kComponentDock), nullptr, true)) {
      dispatcher.push<ToggleComponentDockEvent>();
    }

    if (ImGui::MenuItem(language.get(NounLabel::kLayerDock), nullptr, true)) {
      dispatcher.push<ToggleLayerDockEvent>();
    }

    if (ImGui::MenuItem(language.get(NounLabel::kTilesetDock), nullptr, true)) {
      dispatcher.push<ToggleTilesetDockEvent>();
    }

    if (ImGui::MenuItem(language.get(NounLabel::kAnimationDock), nullptr, true)) {
      dispatcher.push<ToggleAnimationDockEvent>();
    }

    if (ImGui::MenuItem(language.get(NounLabel::kLogDock), nullptr, true)) {
      dispatcher.push<ToggleLogDockEvent>();
    }
  }
}

void _push_font_menu(const Language& language, EventDispatcher& dispatcher)
{
  if (const MenuScope font_menu {language.get(NounLabel::kFont)}; font_menu.is_open()) {
    if (ImGui::MenuItem(language.get(NounLabel::kDefault))) {
      dispatcher.push<SetFontEvent>(FontID::kDefault);
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Roboto")) {
      dispatcher.push<SetFontEvent>(FontID::kRoboto);
    }
  }
}

void _push_theme_menu(const Language& language, EventDispatcher& dispatcher)
{
  if (const MenuScope theme_menu {language.get(NounLabel::kThemeMenu)}; theme_menu.is_open()) {
    if (ImGui::MenuItem("Dear ImGui##Light")) {
      ImGui::StyleColorsLight(&ImGui::GetStyle());
      dispatcher.push<SetThemeEvent>(Theme::kDearImGuiLight);
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Dear ImGui##Dark")) {
      ImGui::StyleColorsDark(&ImGui::GetStyle());
      dispatcher.push<SetThemeEvent>(Theme::kDearImGuiDark);
    }
  }
}

void _push_view_menu(const Model& model, EventDispatcher& dispatcher)
{
  const auto& language = model.get_language();
  const auto* document = model.get_current_document();

  const auto root_entity = (document != nullptr)
                               ? document->get_registry().get<CDocumentInfo>().root
                               : kInvalidEntity;
  const auto content_size = (document != nullptr) ? document->get_content_size() : Float2 {};

  if (const MenuScope menu {language.get(NounLabel::kViewMenu)}; menu.is_open()) {
    _push_widgets_menu(language, dispatcher);

    if (ImGui::MenuItem(language.get(ActionLabel::kToggleUi))) {
      dispatcher.push<ToggleUiEvent>();
    }

    ImGui::Separator();

    _push_theme_menu(language, dispatcher);

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kIncreaseFontSize),
                        kIncreaseFontSizeShortcut.hint)) {
      dispatcher.push<IncreaseFontSizeEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kDecreaseFontSize),
                        kDecreaseFontSizeShortcut.hint)) {
      dispatcher.push<DecreaseFontSizeEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kResetFontSize))) {
      dispatcher.push<ResetFontSizeEvent>();
    }

    _push_font_menu(language, dispatcher);

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kIncreaseZoom),
                        kIncreaseZoomShortcut.hint,
                        false,
                        document != nullptr)) {
      dispatcher.push<IncreaseViewportZoomEvent>(root_entity);
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kDecreaseZoom),
                        kDecreaseZoomShortcut.hint,
                        false,
                        document != nullptr)) {
      dispatcher.push<DecreaseViewportZoomEvent>(root_entity);
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kResetZoom),
                        nullptr,
                        false,
                        document != nullptr)) {
      dispatcher.push<ResetViewportZoomEvent>(root_entity);
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kCenterViewport),
                        kCenterViewportShortcut.hint,
                        false,
                        document != nullptr)) {
      dispatcher.push<CenterViewportEvent>(root_entity, content_size);
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kToggleGrid), kToggleGridShortcut.hint)) {
      dispatcher.push<ToggleGridEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kPanUp),
                        kPanUpShortcut.hint,
                        false,
                        document != nullptr)) {
      dispatcher.push<PanViewportUpEvent>(root_entity);
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kPanDown),
                        kPanDownShortcut.hint,
                        false,
                        document != nullptr)) {
      dispatcher.push<PanViewportDownEvent>(root_entity);
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kPanLeft),
                        kPanLeftShortcut.hint,
                        false,
                        document != nullptr)) {
      dispatcher.push<PanViewportLeftEvent>(root_entity);
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kPanRight),
                        kPanRightShortcut.hint,
                        false,
                        document != nullptr)) {
      dispatcher.push<PanViewportRightEvent>(root_entity);
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kHighlightActiveLayer))) {
      dispatcher.push<ToggleLayerHighlightEvent>();
    }
  }
}

void _push_export_as_menu(const Language& language, EventDispatcher& dispatcher)
{
  if (const MenuScope export_as_menu {language.get(NounLabel::kExportAsMenu)};
      export_as_menu.is_open()) {
    if (ImGui::MenuItem("Tiled JSON (TMJ)")) {
      // TODO
    }

    if (ImGui::MenuItem("Tiled XML (TMX)")) {
      // TODO
    }

    if (ImGui::MenuItem("Godot Scene")) {
      dispatcher.push<ShowGodotExportDialogEvent>();
    }
  }
}

void _push_map_menu(const Model& model, EventDispatcher& dispatcher)
{
  const auto& language = model.get_language();

  const auto* map_document = dynamic_cast<const MapDocument*>(model.get_current_document());
  const DisabledScope disable_menu_if {map_document == nullptr};

  if (const MenuScope menu {language.get(NounLabel::kMapMenu)}; menu.is_open()) {
    const auto* registry = map_document ? &map_document->get_registry() : nullptr;
    const auto* document_info = registry ? registry->find<CDocumentInfo>() : nullptr;
    const auto* map = registry ? registry->find<CMap>(document_info->root) : nullptr;

    if (ImGui::MenuItem(language.get(ActionLabel::kShowMetadata))) {
      dispatcher.push<InspectMapEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kAddTileset))) {
      dispatcher.push<ShowAddTilesetDialogEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kResize))) {
      dispatcher.push<ShowResizeMapDialogEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kAddRow))) {
      dispatcher.push<AddRowToMapEvent>();
    }

    if (ImGui::MenuItem(language.get(ActionLabel::kAddColumn))) {
      dispatcher.push<AddColumnToMapEvent>();
    }

    if (const DisabledScope disable_if {!map || map->extent.rows <= 1};
        ImGui::MenuItem(language.get(ActionLabel::kRemoveRow))) {
      dispatcher.push<RemoveRowToMapEvent>();
    }

    if (const DisabledScope disable_if {!map || map->extent.cols <= 1};
        ImGui::MenuItem(language.get(ActionLabel::kRemoveColumn))) {
      dispatcher.push<RemoveColumnToMapEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kFixInvalidTiles))) {
      dispatcher.push<FixMapTilesEvent>();
    }

    ImGui::Separator();

    _push_export_as_menu(language, dispatcher);
  }
}

void _push_tileset_menu(const Model& model, EventDispatcher& dispatcher)
{
  const auto& language = model.get_language();
  const MenuScope menu {language.get(NounLabel::kTilesetMenu)};
  if (menu.is_open()) {
    // TODO
  }
}

void _push_help_menu(const Model& model, EventDispatcher& dispatcher)
{
  const auto& language = model.get_language();

  if (const MenuScope menu {language.get(NounLabel::kHelpMenu)}; menu.is_open()) {
    if (ImGui::MenuItem(language.get(ActionLabel::kAboutTactile))) {
      dispatcher.push<ShowAboutDialogEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kReportBug))) {
      if (SDL_OpenURL("https://github.com/albin-johansson/tactile/issues/new") != 0) {
        TACTILE_CORE_ERROR("Could not open issue URL: {}", SDL_GetError());
      }
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kOpenCredits))) {
      dispatcher.push<ShowCreditsDialogEvent>();
    }
  }
}

}  // namespace

void MenuBar::push(const Model& model, EventDispatcher& dispatcher)
{
  if (ImGui::BeginMainMenuBar()) {
    _push_file_menu(model, dispatcher);
    _push_edit_menu(model, dispatcher);
    _push_view_menu(model, dispatcher);
    _push_map_menu(model, dispatcher);
    _push_tileset_menu(model, dispatcher);
    _push_help_menu(model, dispatcher);
    _push_debug_menu(model);

    ImGui::EndMainMenuBar();
  }
}

void MenuBar::_push_debug_menu(const Model& model)
{
  const auto& language = model.get_language();

  const MenuScope menu {language.get(NounLabel::kDebugMenu)};
  if (menu.is_open()) {
    if (ImGui::MenuItem(language.get(ActionLabel::kOpenDebugger), nullptr, m_show_debugger)) {
      m_show_debugger = !m_show_debugger;
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kOpenStyleEditor),
                        nullptr,
                        m_show_style_editor)) {
      m_show_style_editor = !m_show_style_editor;
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kOpenDemoWindow), nullptr, m_show_demo)) {
      m_show_demo = !m_show_demo;
    }

    ImGui::Separator();

    if (ImGui::MenuItem(language.get(ActionLabel::kOpenStorageDir))) {
      if (const auto persistent_dir = get_persistent_storage_directory()) {
        std::ignore = open_directory_in_finder(*persistent_dir);
      }
    }
  }

  if (m_show_debugger) {
    ImGui::ShowMetricsWindow(&m_show_debugger);
  }

  if (m_show_demo) {
    ImGui::ShowDemoWindow(&m_show_demo);
  }

  if (m_show_style_editor) {
    const Window style_editor {language.get(NounLabel::kStyleEditorWidget),
                               ImGuiWindowFlags_None,
                               &m_show_style_editor};
    if (style_editor.is_open()) {
      ImGui::ShowStyleEditor();
    }
  }
}

}  // namespace tactile::core::ui
