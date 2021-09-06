#pragma once

#include <centurion.hpp>  // window, keyboard
#include <entt.hpp>       // dispatcher

#include "aliases/ints.hpp"
#include "core/model.hpp"
#include "events/command_events.hpp"
#include "events/layer_events.hpp"
#include "events/map_events.hpp"
#include "events/property_events.hpp"
#include "events/quit_event.hpp"
#include "events/save_as_event.hpp"
#include "events/tileset_events.hpp"
#include "events/tool_events.hpp"
#include "events/viewport_events.hpp"
#include "gui/icons.hpp"

namespace Tactile {

class Application final
{
  friend void SubscribeToEvents(Application*, entt::dispatcher&);

 public:
  Application(cen::window&& window);

  auto Run() -> int;

 private:
  cen::window mWindow;
  cen::keyboard mKeyboard;
  entt::dispatcher mDispatcher;

  Model mModel;
  [[no_unique_address]] Icons mIcons;

  bool mQuit{};

  void OnAboutToExit();

  void PollEvents();

  void UpdateFrame();

  void OnAddMapEvent(const AddMapEvent& event);

  void OnCloseMapEvent(const CloseMapEvent& event);

  void OnOpenMapEvent(const OpenMapEvent& event);

  void OnSaveEvent();

  void OnSaveAsEvent(const SaveAsEvent& event);

  void OnSaveAsRequestEvent();

  void OnAddTilesetEvent(const AddTilesetEvent& event);

  void OnUndoEvent();

  void OnRedoEvent();

  void OnSelectToolEvent(const SelectToolEvent& event);

  void OnMousePressedEvent(const MousePressedEvent& event);

  void OnMouseReleasedEvent(const MouseReleasedEvent& event);

  void OnMouseDragEvent(const MouseDragEvent& event);

  void OnStampSequenceEvent(StampSequenceEvent event);

  void OnEraserSequenceEvent(EraserSequenceEvent event);

  void OnFloodEvent(const FloodEvent& event);

  void OnCenterViewportEvent();

  void OnOffsetViewportEvent(const OffsetViewportEvent& event);

  void OnPanLeftEvent();

  void OnPanRightEvent();

  void OnPanUpEvent();

  void OnPanDownEvent();

  void OnIncreaseViewportZoomEvent();

  void OnDecreaseViewportZoomEvent();

  void OnResetViewportZoomEvent();

  void OnSelectMapEvent(const SelectMapEvent& event);

  void OnSelectLayerEvent(const SelectLayerEvent& event);

  void OnSelectTilesetEvent(const SelectTilesetEvent& event);

  void OnRemoveTilesetEvent(const RemoveTilesetEvent& event);

  void OnAddRowEvent();

  void OnAddColumnEvent();

  void OnRemoveRowEvent();

  void OnRemoveColumnEvent();

  void OnAddLayerEvent(const AddLayerEvent& event);

  void OnRemoveLayerEvent(const RemoveLayerEvent& event);

  void OnMoveLayerUpEvent(const MoveLayerUpEvent& event);

  void OnMoveLayerDownEvent(const MoveLayerDownEvent& event);

  void OnDuplicateLayerEvent(const DuplicateLayerEvent& event);

  void OnSetLayerOpacityEvent(const SetLayerOpacityEvent& event);

  void OnSetLayerVisibleEvent(const SetLayerVisibleEvent& event);

  void OnShowLayerPropertiesEvent(const ShowLayerPropertiesEvent& event);

  void OnShowMapPropertiesEvent();

  void OnAddPropertyEvent(const AddPropertyEvent& event);

  void OnRemovePropertyEvent(const RemovePropertyEvent& event);

  void OnRenamePropertyEvent(const RenamePropertyEvent& event);

  void OnSetPropertyValueEvent(const SetPropertyValueEvent& event);

  void OnChangePropertyTypeEvent(const ChangePropertyTypeEvent& event);

  void OnSetTilesetSelectionEvent(const SetTilesetSelectionEvent& event);

  void OnChangeCommandCapacityEvent(const ChangeCommandCapacityEvent& event);

  void OnQuitEvent();
};

}  // namespace Tactile
