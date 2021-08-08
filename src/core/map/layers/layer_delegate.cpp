#include "layer_delegate.hpp"

#include <algorithm>  // clamp
#include <utility>    // move

namespace Tactile {

LayerDelegate::LayerDelegate(const LayerType type) : mType{type}
{}

void LayerDelegate::SetVisible(const bool visible) noexcept
{
  mVisible = visible;
}

void LayerDelegate::SetOpacity(const float opacity)
{
  mOpacity = std::clamp(opacity, 0.0f, 1.0f);
}

void LayerDelegate::SetName(std::string name)
{
  mProperties.SetName(std::move(name));
}

void LayerDelegate::AddProperty(std::string name, const PropertyType type)
{
  mProperties.AddProperty(std::move(name), type);
}

void LayerDelegate::AddProperty(std::string name, const Property& property)
{
  mProperties.AddProperty(std::move(name), property);
}

void LayerDelegate::RemoveProperty(const std::string_view name)
{
  mProperties.RemoveProperty(name);
}

void LayerDelegate::RenameProperty(const std::string_view oldName,
                                   std::string newName)
{
  mProperties.RenameProperty(oldName, std::move(newName));
}

void LayerDelegate::SetProperty(const std::string_view name,
                                const Property& property)
{
  mProperties.SetProperty(name, property);
}

void LayerDelegate::ChangePropertyType(std::string name, const PropertyType type)
{
  mProperties.ChangePropertyType(std::move(name), type);
}

auto LayerDelegate::HasProperty(const std::string_view name) const -> bool
{
  return mProperties.HasProperty(name);
}

auto LayerDelegate::GetProperty(const std::string_view name) const -> const Property&
{
  return mProperties.GetProperty(name);
}

auto LayerDelegate::GetProperties() const -> const PropertyMap&
{
  return mProperties.GetProperties();
}

auto LayerDelegate::GetPropertyCount() const -> usize
{
  return mProperties.GetPropertyCount();
}

auto LayerDelegate::GetName() const -> const std::string&
{
  return mProperties.GetName();
}

}  // namespace Tactile
