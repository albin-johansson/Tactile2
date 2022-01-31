#include "change_property_type_cmd.hpp"

#include <utility>  // move

#include "core/systems/property_system.hpp"

namespace tactile {

ChangePropertyTypeCmd::ChangePropertyTypeCmd(registry_ref registry,
                                             std::string name,
                                             attribute_type type)
    : command_base{"Change Property Type"}
    , mRegistry{registry}
    , mContextId{sys::GetCurrentContextId(mRegistry)}
    , mName{std::move(name)}
    , mPropertyType{type}
{}

void ChangePropertyTypeCmd::undo()
{
  const auto& value = mPreviousValue.value();
  const auto type = value.type();

  auto& context = sys::GetContext(mRegistry, mContextId);
  sys::ChangePropertyType(mRegistry, context, mName, type);
  sys::UpdateProperty(mRegistry, context, mName, value);

  mPreviousValue.reset();
}

void ChangePropertyTypeCmd::redo()
{
  auto& context = sys::GetContext(mRegistry, mContextId);
  mPreviousValue = sys::GetProperty(mRegistry, context, mName).value;
  sys::ChangePropertyType(mRegistry, context, mName, mPropertyType);
}

}  // namespace tactile
