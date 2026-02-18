/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the InputMappingRegistry class
/////////////////////////////////////////////////

#include "InputMappingRegistry.h"

namespace steamrot::events {

/////////////////////////////////////////////////
InputMappingRegistry::InputMappingRegistry() { InitializeDefaultMappings(); }

/////////////////////////////////////////////////
void InputMappingRegistry::MapKeyboardKey(sf::Keyboard::Key key,
                                          InputPayload::InputAction action) {
  m_keyboard_mappings[key] = action;
}

/////////////////////////////////////////////////
void InputMappingRegistry::MapMouseButton(sf::Mouse::Button button,
                                          InputPayload::InputAction action) {
  m_mouse_mappings[button] = action;
}

/////////////////////////////////////////////////
InputPayload::InputAction
InputMappingRegistry::GetActionForKey(sf::Keyboard::Key key) const {
  auto it = m_keyboard_mappings.find(key);
  if (it != m_keyboard_mappings.end()) {
    return it->second;
  }
  return InputPayload::InputAction::NONE;
}

/////////////////////////////////////////////////
InputPayload::InputAction
InputMappingRegistry::GetActionForMouseButton(sf::Mouse::Button button) const {
  auto it = m_mouse_mappings.find(button);
  if (it != m_mouse_mappings.end()) {
    return it->second;
  }
  return InputPayload::InputAction::NONE;
}

/////////////////////////////////////////////////
void InputMappingRegistry::UnmapKeyboardKey(sf::Keyboard::Key key) {
  m_keyboard_mappings.erase(key);
}

/////////////////////////////////////////////////
void InputMappingRegistry::UnmapMouseButton(sf::Mouse::Button button) {
  m_mouse_mappings.erase(button);
}

/////////////////////////////////////////////////
void InputMappingRegistry::ClearKeyboardMappings() {
  m_keyboard_mappings.clear();
}

/////////////////////////////////////////////////
void InputMappingRegistry::ClearMouseMappings() { m_mouse_mappings.clear(); }

/////////////////////////////////////////////////
void InputMappingRegistry::ResetToDefaults() {
  ClearKeyboardMappings();
  ClearMouseMappings();
  InitializeDefaultMappings();
}

/////////////////////////////////////////////////
void InputMappingRegistry::InitializeDefaultMappings() {
  // Default keyboard mappings
  // Map Enter/Return key to SELECT action
  m_keyboard_mappings[sf::Keyboard::Key::Enter] =
      InputPayload::InputAction::SELECT;
  m_keyboard_mappings[sf::Keyboard::Key::Space] =
      InputPayload::InputAction::SELECT;

  // Default mouse mappings
  // Map left mouse button to SELECT action
  m_mouse_mappings[sf::Mouse::Button::Left] =
      InputPayload::InputAction::SELECT;
}

} // namespace steamrot::events
