#pragma once

#include <string>  // string

#include <entt/entt.hpp>

#include "tactile_def.hpp"

namespace tactile {

/// \addtogroup commands
/// \{

/**
 * \brief All commands are expected to take this as their first constructor parameter.
 */
using registry_ref = ref<entt::registry>;

/**
 * \brief The abstract base class of all command implementations.
 */
class command_base {
 public:
  TACTILE_DELETE_COPY(command_base)
  TACTILE_DEFAULT_MOVE(command_base)

  command_base() = default;

  /**
   * \brief Creates a command.
   *
   * \param text a short human-readable command name.
   */
  explicit command_base(std::string text);

  virtual ~command_base() = default;

  /**
   * \brief Reverts the effect of the command.
   *
   * \details This function should revert the effects of having previously
   * called `redo()`.
   */
  virtual void undo() = 0;

  /**
   * \brief Applies the effects of the command.
   *
   * \details This function is called whenever a command is pushed onto the
   * command stack.
   *
   * \see `CommandStack::Push`
   */
  virtual void redo() = 0;

  /**
   * \brief Returns a identifier unique for the command type.
   *
   * \details This function is mainly designed to be used when overriding the
   * `merge_with()` function, where it can be used to efficiently test if two
   * commands are of the same type, since the parameter type is `ACommand`.
   *
   * \return an identifier unique to the command class.
   *
   * \see `CommandId`
   */
  [[nodiscard]] virtual auto id() const -> int = 0;

  /**
   * \brief Attempts to merge the supplied command into *this* command.
   *
   * \details Override this function in order to enable reducing the amount of
   * commands on the command stack when it makes sense to chain a group of
   * commands of the same type together. For example, this is used to combine
   * consecutive "Add Row" commands.
   *
   * \details Use the `id()` function to efficiently filter out commands of
   * differing types.
   *
   * \note This function is only called on the command on top of the command
   * stack.
   *
   * \param cmd the command that will potentially be merged into *this* command.
   *
   * \return `true` if the supplied command was merged into *this* command;
   * `false` otherwise.
   */
  [[nodiscard]] virtual auto merge_with([[maybe_unused]] const command_base& cmd) -> bool
  {
    return false;
  }

  /**
   * \brief Returns a short human-readable string that describes the command.
   *
   * \return the name of the command.
   */
  [[nodiscard]] auto text() const -> const std::string& { return mText; }

 private:
  std::string mText;
};

/// \} End of group commands

}  // namespace tactile
