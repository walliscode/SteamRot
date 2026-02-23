/////////////////////////////////////////////////
/// @file
/// @brief Static configuration types for mapping SFML inputs to InputPayloads.
///
/// This file defines the data structures used to configure the
/// SFMLInputRegistry. It serves as the static, code-defined alternative to a
/// FlatBuffers schema, making it straightforward to replace with
/// FlatBuffers-driven data when serialisation/deserialisation is needed.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayload.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Represents a single physical input requirement within a binding.
///
/// An SFMLInputEntry is either a keyboard key or a mouse button. Multiple
/// entries in one SFMLInputBinding are combined with AND logic — all must be
/// simultaneously held for the binding to be satisfied.
/////////////////////////////////////////////////
struct SFMLInputEntry {

  /////////////////////////////////////////////////
  /// @brief Distinguishes between keyboard and mouse button inputs
  /////////////////////////////////////////////////
  enum class Type { Keyboard, MouseButton } type{Type::Keyboard};

  /////////////////////////////////////////////////
  /// @brief The keyboard key (meaningful only when type == Keyboard)
  /////////////////////////////////////////////////
  sf::Keyboard::Key keyboard_key{sf::Keyboard::Key::Unknown};

  /////////////////////////////////////////////////
  /// @brief The mouse button (meaningful only when type == MouseButton)
  /////////////////////////////////////////////////
  sf::Mouse::Button mouse_button{sf::Mouse::Button::Left};
};

/////////////////////////////////////////////////
/// @brief Maps a combination of SFML inputs to an InputPayload action.
///
/// All entries in required_inputs must be simultaneously held (AND logic) for
/// the binding to be considered satisfied. The trigger_state controls whether
/// the action fires when the combination is first fully pressed (PRESSED) or
/// when any of the required inputs is released after being fully pressed
/// (RELEASED).
/////////////////////////////////////////////////
struct SFMLInputBinding {

  /////////////////////////////////////////////////
  /// @brief The action to produce when this binding is triggered
  /////////////////////////////////////////////////
  InputPayload::InputAction action{InputPayload::InputAction::NONE};

  /////////////////////////////////////////////////
  /// @brief Whether to fire on the combination becoming fully pressed or
  /// on any required input being released after the combination was active
  /////////////////////////////////////////////////
  InputPayload::InputState trigger_state{InputPayload::InputState::PRESSED};

  /////////////////////////////////////////////////
  /// @brief All physical inputs that must be held simultaneously (AND logic)
  /////////////////////////////////////////////////
  std::vector<SFMLInputEntry> required_inputs;
};

/////////////////////////////////////////////////
/// @brief Returns the default static SFML input bindings.
///
/// Provides an initial set of bindings without requiring any external data
/// source. When FlatBuffers support is added, this can be replaced by loading
/// bindings from a binary data file via the same SFMLInputRegistry::Configure()
/// interface.
///
/// @return Vector of default SFMLInputBindings
/////////////////////////////////////////////////
inline std::vector<SFMLInputBinding> GetDefaultSFMLInputBindings() {
  using Key = sf::Keyboard::Key;
  using Button = sf::Mouse::Button;
  using Entry = SFMLInputEntry;
  using EntryType = SFMLInputEntry::Type;
  using Action = InputPayload::InputAction;
  using State = InputPayload::InputState;

  return {
      SFMLInputBinding{
          Action::SELECT,
          State::PRESSED,
          {Entry{EntryType::MouseButton, Key::Unknown, Button::Left}}},
      SFMLInputBinding{
          Action::SELECT,
          State::RELEASED,
          {Entry{EntryType::MouseButton, Key::Unknown, Button::Left}}},
  };
}

} // namespace steamrot
