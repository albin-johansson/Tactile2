# Command API

The undo/redo support is provided using a typical command pattern architecture, where all commands are implemented as
derived classes of the `ACommand` abstract class. All command implementations must provide `undo()`, `redo()`,
and `id()` functions. Additionally, the command stack is able to merge consecutive commands of the same type for command
types that override `merge_with()`. See the in-source documentation for more details regarding these functions.

## Guidelines

* Never store entity identifiers in commands, use dedicated identifiers such as `ObjectID` instead.
* Always take `RegistryRef` as the first command constructor parameter.
* Use `id()` to perform initial type checks when overriding `merge_with()`, but use `dynamic_cast` and not `static_cast`
  for the actual downcast.

## Adding a new command

The process of adding a new command to the application always involves a set of steps which are listed in this section.

1. Add an enumerator for the new command to the `command_id` enum.
2. Implement the new command class, make sure that the `id()` function returns the newly added enumerator value.
3. Most commands are executed as a result of an event, so make sure that `Application` has a function that handles the
   event.
4. Make sure that the dispatcher connects the event to the appropriate `Application` function,
   see `application_events.cpp`.
5. Implement the creation of the command using one of the `_execute()` or `_register()` functions in `application.cpp`,
   which adds a command to the command stack. `_execute()` invokes the `redo()` function of the added command
   immediately (this is the most common case), `_register()` is used for commands whose effects have already been
   applied, as with the stamp tool, for example.