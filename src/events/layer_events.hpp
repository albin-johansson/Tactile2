#pragma once

#include <string>  // string

#include "common/layer_id.hpp"
#include "core/layer_type.hpp"

namespace Tactile {

/// \addtogroup events
/// \{

struct AddLayerEvent final
{
  LayerType type;
};

struct RemoveLayerEvent final
{
  LayerID id;
};

struct DuplicateLayerEvent final
{
  LayerID id;
};

struct SelectLayerEvent final
{
  LayerID id;
};

struct MoveLayerUpEvent final
{
  LayerID id;
};

struct MoveLayerDownEvent final
{
  LayerID id;
};

struct SetLayerOpacityEvent final
{
  LayerID id;
  float opacity{};
};

struct SetLayerVisibleEvent final
{
  LayerID id;
  bool visible{};
};

struct ShowLayerPropertiesEvent final
{
  LayerID id;
};

struct OpenRenameLayerDialogEvent final
{
  LayerID id;
};

struct RenameLayerEvent final
{
  LayerID id;
  std::string name;
};

/// \} End of group events

}  // namespace Tactile
