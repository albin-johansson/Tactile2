#include "string_property_widget.hpp"

#include <array>   // array
#include <limits>  // numeric_limits

#include "core/utils/buffer_utils.hpp"
#include "editor/gui/widgets/scoped.hpp"

namespace Tactile {
namespace {

constexpr usize gMaxDigits = 100;

}  // namespace

auto StringPropertyWidget(const PropertyValue& property,
                          const ImGuiInputTextFlags flags,
                          ImGuiInputTextCallback filter) -> Maybe<std::string>
{
  const Scoped::ID scope{&property};
  const auto& str = property.AsString();

  std::array<char, gMaxDigits> buffer;  // NOLINT safely uninitialized
  CopyStringIntoBuffer(buffer, str);

  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
  if (ImGui::InputTextWithHint("##StringPropertyInput",
                               "N/A",
                               buffer.data(),
                               sizeof buffer,
                               flags,
                               filter))
  {
    return CreateStringFromBuffer(buffer);
  }

  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("[string]");
  }

  return nothing;
}

}  // namespace Tactile
