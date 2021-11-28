#include "undo_shortcut.hpp"

#include "editor/events/command_events.hpp"
#include "editor/model.hpp"

namespace Tactile {

UndoShortcut::UndoShortcut() : AShortcut{cen::scancodes::z, KMOD_LCTRL}
{}

void UndoShortcut::Activate(entt::dispatcher& dispatcher)
{
  dispatcher.enqueue<UndoEvent>();
}

auto UndoShortcut::IsEnabled(const Model& model) const -> bool
{
  return model.CanUndo();
}

}  // namespace Tactile
