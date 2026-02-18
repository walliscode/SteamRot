/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for converting SFML events to InputPayload
/////////////////////////////////////////////////

#include "sfml_input_converter.h"

namespace steamrot::events {

/////////////////////////////////////////////////
std::optional<InputPayload>
ConvertSFMLEventToInputPayload(const sf::Event &event,
                               const InputMappingRegistry &registry) {
  // Handle keyboard events
  if (const auto *keyPressed = event.getIf<sf::Event::KeyPressed>()) {
    InputPayload::InputAction action =
        registry.GetActionForKey(keyPressed->code);
    if (action != InputPayload::InputAction::NONE) {
      return InputPayload{action};
    }
  }
  // Handle mouse button events
  else if (const auto *mousePressed =
               event.getIf<sf::Event::MouseButtonPressed>()) {
    InputPayload::InputAction action =
        registry.GetActionForMouseButton(mousePressed->button);
    if (action != InputPayload::InputAction::NONE) {
      return InputPayload{action};
    }
  }

  // Event doesn't map to any InputAction
  return std::nullopt;
}

/////////////////////////////////////////////////
std::optional<InputPayload>
ConvertSFMLEventToInputPayload(const sf::Event &event) {
  // Create a temporary registry with default mappings
  InputMappingRegistry registry;
  return ConvertSFMLEventToInputPayload(event, registry);
}

} // namespace steamrot::events
