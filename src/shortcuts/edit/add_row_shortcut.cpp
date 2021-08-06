#include "add_row_shortcut.hpp"

#include "events/maps/add_row_event.hpp"

namespace Tactile {

AddRowShortcut::AddRowShortcut()
    : AShortcut{cen::scancodes::r, cen::key_mod::left_alt}
{}

void AddRowShortcut::Activate(entt::dispatcher& dispatcher)
{
  dispatcher.enqueue<AddRowEvent>();
}

}  // namespace Tactile
