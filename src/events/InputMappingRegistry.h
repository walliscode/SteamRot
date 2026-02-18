/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the InputMappingRegistry class for mapping SFML inputs to InputActions
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
#include <unordered_map>

namespace steamrot::events {

/////////////////////////////////////////////////
/// @class InputMappingRegistry
/// @brief Extensible registry for mapping SFML keyboard and mouse inputs to InputActions
///
/// This class provides a runtime-configurable mapping system that allows
/// users to customize which keyboard keys and mouse buttons trigger which
/// InputActions. The registry can be modified at runtime to support
/// user-defined control schemes.
/////////////////////////////////////////////////
class InputMappingRegistry {
public:
  /////////////////////////////////////////////////
  /// @brief Default constructor initializes registry with default mappings
  /////////////////////////////////////////////////
  InputMappingRegistry();

  /////////////////////////////////////////////////
  /// @brief Maps a keyboard key to an InputAction
  ///
  /// @param key The SFML keyboard key to map
  /// @param action The InputAction to associate with the key
  /////////////////////////////////////////////////
  void MapKeyboardKey(sf::Keyboard::Key key, InputPayload::InputAction action);

  /////////////////////////////////////////////////
  /// @brief Maps a mouse button to an InputAction
  ///
  /// @param button The SFML mouse button to map
  /// @param action The InputAction to associate with the button
  /////////////////////////////////////////////////
  void MapMouseButton(sf::Mouse::Button button,
                      InputPayload::InputAction action);

  /////////////////////////////////////////////////
  /// @brief Gets the InputAction associated with a keyboard key
  ///
  /// @param key The SFML keyboard key to look up
  /// @return The associated InputAction, or InputAction::NONE if not mapped
  /////////////////////////////////////////////////
  InputPayload::InputAction GetActionForKey(sf::Keyboard::Key key) const;

  /////////////////////////////////////////////////
  /// @brief Gets the InputAction associated with a mouse button
  ///
  /// @param button The SFML mouse button to look up
  /// @return The associated InputAction, or InputAction::NONE if not mapped
  /////////////////////////////////////////////////
  InputPayload::InputAction
  GetActionForMouseButton(sf::Mouse::Button button) const;

  /////////////////////////////////////////////////
  /// @brief Removes a keyboard key mapping
  ///
  /// @param key The SFML keyboard key to unmap
  /////////////////////////////////////////////////
  void UnmapKeyboardKey(sf::Keyboard::Key key);

  /////////////////////////////////////////////////
  /// @brief Removes a mouse button mapping
  ///
  /// @param button The SFML mouse button to unmap
  /////////////////////////////////////////////////
  void UnmapMouseButton(sf::Mouse::Button button);

  /////////////////////////////////////////////////
  /// @brief Clears all keyboard key mappings
  /////////////////////////////////////////////////
  void ClearKeyboardMappings();

  /////////////////////////////////////////////////
  /// @brief Clears all mouse button mappings
  /////////////////////////////////////////////////
  void ClearMouseMappings();

  /////////////////////////////////////////////////
  /// @brief Resets all mappings to default values
  /////////////////////////////////////////////////
  void ResetToDefaults();

private:
  /////////////////////////////////////////////////
  /// @brief Initializes default keyboard and mouse mappings
  /////////////////////////////////////////////////
  void InitializeDefaultMappings();

  std::unordered_map<sf::Keyboard::Key, InputPayload::InputAction>
      m_keyboard_mappings;
  std::unordered_map<sf::Mouse::Button, InputPayload::InputAction>
      m_mouse_mappings;
};

} // namespace steamrot::events
