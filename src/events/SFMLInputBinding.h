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
/// An SFMLInputEntry is either a keyboard key or a mouse button, together with
/// whether its pressed or released state must be detected for this entry to
/// contribute to satisfying the binding.
///
/// Multiple entries in one SFMLInputBinding are combined with AND logic — all
/// must be triggered together for the binding to fire.
/////////////////////////////////////////////////
struct SFMLInputEntry {

  /////////////////////////////////////////////////
  /// @brief Distinguishes between keyboard and mouse button inputs
  /////////////////////////////////////////////////
  enum class Type { Keyboard, MouseButton } type{Type::Keyboard};

  /////////////////////////////////////////////////
  /// @brief Whether this entry requires the input to be pressed or released
  /////////////////////////////////////////////////
  enum class TriggerOn { Pressed, Released } trigger_on{TriggerOn::Pressed};

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
/// All entries in required_inputs must be simultaneously satisfied (AND logic)
/// for the binding to fire. Each entry specifies which physical input it
/// requires and whether it should be in a pressed or released state.
///
/// The resulting InputPayload carries only the action — the SFML-level
/// press/release distinction is an internal binding concern.
/////////////////////////////////////////////////
struct SFMLInputBinding {

  /////////////////////////////////////////////////
  /// @brief The action to produce when this binding fires
  /////////////////////////////////////////////////
  InputPayload::InputAction action{InputPayload::InputAction::NONE};

  /////////////////////////////////////////////////
  /// @brief All physical inputs that must be satisfied simultaneously (AND logic)
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
  using TriggerOn = SFMLInputEntry::TriggerOn;
  using Action = InputPayload::InputAction;

  return {
      // Left mouse button press → SELECT
      SFMLInputBinding{
          Action::SELECT,
          {Entry{EntryType::MouseButton, TriggerOn::Pressed, Key::Unknown,
                 Button::Left}}},
      // Left mouse button release → SELECT
      SFMLInputBinding{
          Action::SELECT,
          {Entry{EntryType::MouseButton, TriggerOn::Released, Key::Unknown,
                 Button::Left}}},
  };
}

} // namespace steamrot
