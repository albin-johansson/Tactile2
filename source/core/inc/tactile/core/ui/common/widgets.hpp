// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <imgui.h>

#include "tactile/base/int.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/core/entity/entity.hpp"
#include "tactile/core/numeric/vec.hpp"

namespace tactile::ui {

/// \addtogroup UI
/// \{

/**
 * RAII helper for the PushID/PopID ImGui functions.
 */
class IdScope final
{
 public:
  TACTILE_DELETE_COPY(IdScope);
  TACTILE_DELETE_MOVE(IdScope);

  [[nodiscard]]
  explicit IdScope(const void* id);

  [[nodiscard]]
  explicit IdScope(const char* id);

  [[nodiscard]]
  explicit IdScope(int id);

  [[nodiscard]]
  explicit IdScope(EntityID id);

  ~IdScope() noexcept;
};

/**
 * RAII helper for the BeginDisabled/EndDisabled ImGui functions.
 */
class DisabledScope final
{
 public:
  TACTILE_DELETE_COPY(DisabledScope);
  TACTILE_DELETE_MOVE(DisabledScope);

  [[nodiscard]]
  explicit DisabledScope(bool disabled = true);

  ~DisabledScope() noexcept;
};

/**
 * RAII helper for the Indent/Unindent ImGui functions.
 */
class IndentScope final
{
 public:
  TACTILE_DELETE_COPY(IndentScope);
  TACTILE_DELETE_MOVE(IndentScope);

  [[nodiscard]]
  IndentScope();

  ~IndentScope() noexcept;
};

/**
 * RAII helper for the BeginGroup/EndGroup ImGui functions.
 */
class GroupScope final
{
 public:
  TACTILE_DELETE_COPY(GroupScope);
  TACTILE_DELETE_MOVE(GroupScope);

  [[nodiscard]]
  GroupScope();

  ~GroupScope() noexcept;
};

/**
 * RAII helper for the BeginTooltip/EndTooltip ImGui functions.
 */
class TooltipScope final
{
 public:
  TACTILE_DELETE_COPY(TooltipScope);
  TACTILE_DELETE_MOVE(TooltipScope);

  [[nodiscard]]
  TooltipScope();

  ~TooltipScope() noexcept;
};

/**
 * Calculates the size of a standard widget (e.g., a button) with a given label.
 *
 * \param text The widget label.
 *
 * \return
 * The computed widget size.
 */
[[nodiscard]]
auto get_widget_size(const char* text) -> Float2;

/**
 * Centers the next widget horizontally.
 *
 * \param width The next widget width.
 */
void center_next_widget_horizontally(float width);

/**
 * Centers the next widget vertically.
 *
 * \param height The next widget height.
 */
void center_next_widget_vertically(float height);

/**
 * Centers the next widget both horizontally and vertically.
 *
 * \param size The next widget size.
 */
void center_next_widget(const Float2& size);

/**
 * Prepares the widget cursor for a given number of vertically centered widgets.
 *
 * \param count The number of widgets to accommodate for.
 */
void prepare_for_vertically_centered_widgets(float count);

/// \}

}  // namespace tactile::ui