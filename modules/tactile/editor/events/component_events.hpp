#pragma once

#include <string>  // string

#include "core/attribute_value.hpp"
#include "core/components/component.hpp"
#include "tactile_def.hpp"

namespace tactile {

struct OpenComponentEditorEvent final
{};

struct CreateComponentDefEvent final
{
  std::string name;
};

struct RenameComponentDefEvent final
{
  ComponentID id{};
  std::string name;
};

struct RemoveComponentDefEvent final
{
  ComponentID id{};
};

struct CreateComponentAttributeEvent final
{
  ComponentID id{};
  std::string name;
};

struct RemoveComponentAttributeEvent final
{
  ComponentID id{};
  std::string name;
};

struct RenameComponentAttributeEvent final
{
  ComponentID id{};
  std::string previous;
  std::string updated;
};

struct DuplicateComponentAttributeEvent final
{
  ComponentID id{};
  std::string attribute;
};

struct SetComponentAttributeTypeEvent final
{
  ComponentID id{};
  std::string attribute;
  attribute_type type{};
};

struct UpdateComponentDefAttributeEvent final
{
  ComponentID id{};
  std::string attribute;
  attribute_value value;
};

struct AddComponentEvent final
{
  ContextID context{};
  ComponentID component{};
};

struct RemoveComponentEvent final
{
  ContextID context{};
  ComponentID component{};
};

struct ResetComponentValuesEvent final
{
  ContextID context{};
  ComponentID component{};
};

struct UpdateComponentEvent final
{
  ContextID context{};
  ComponentID component{};
  std::string attribute;
  attribute_value value;
};

}  // namespace tactile