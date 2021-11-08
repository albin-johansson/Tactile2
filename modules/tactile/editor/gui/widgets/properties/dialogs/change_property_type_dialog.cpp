#include "change_property_type_dialog.hpp"

#include <utility>  // move

#include <tactile_def.hpp>
#include <tactile_stdlib.hpp>

#include <imgui.h>

#include "editor/events/property_events.hpp"
#include "editor/gui/widgets/alignment.hpp"
#include "editor/gui/widgets/common/button.hpp"
#include "editor/gui/widgets/common/modal.hpp"
#include "property_type_combo.hpp"

namespace Tactile {
namespace {

constexpr auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;

constinit PropertyType current_type = PropertyType::String;
inline Maybe<std::string> property_name;
inline Maybe<PropertyType> previous_type;

void ResetState()
{
  current_type = PropertyType::String;
  property_name.reset();
  previous_type.reset();
}

}  // namespace

void UpdateChangePropertyTypeDialog(entt::dispatcher& dispatcher)
{
  CenterNextWindowOnAppearance();
  if (auto modal = Modal{"Change property type", flags}) {
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Type: ");

    ImGui::SameLine();
    PropertyTypeCombo(previous_type.value(), current_type);

    ImGui::Spacing();

    if (Button("OK", nullptr, current_type != previous_type.value())) {
      dispatcher.enqueue<ChangePropertyTypeEvent>(property_name.value(), current_type);

      ResetState();
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      ResetState();
      ImGui::CloseCurrentPopup();
    }
  }
}

void OpenChangePropertyTypeDialog(std::string name, const PropertyType type)
{
  property_name = std::move(name);
  current_type = type;
  previous_type = type;
  ImGui::OpenPopup("Change property type");
}

}  // namespace Tactile
