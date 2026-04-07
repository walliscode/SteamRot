/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for converting SFML events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "sfml_event_convert.h"
#include "InputActionRegistry.h"
#include "UserInputBitset.h"
#include <bitset>

namespace steamrot::events::convert {

/////////////////////////////////////////////////
UserInputBitset
CollectInputEvents(const std::vector<sf::Event> &sfml_events) {
  return UserInputBitset(sfml_events);
}

/////////////////////////////////////////////////
std::optional<InputPayload::InputAction>
ResolveInputAction(const UserInputBitset &accumulated,
                   const InputActionRegistry &registry) {

  for (const auto &[pattern, action] : registry) {

    const auto &pat =
        static_cast<const std::bitset<kTotalBits> &>(pattern);
    const auto &acc =
        static_cast<const std::bitset<kTotalBits> &>(accumulated);

    if ((pat & acc) == pat && pat.any()) {
      return action;
    }
  }

  return std::nullopt;
}

/////////////////////////////////////////////////
bool CollectSystemEvents(const std::vector<sf::Event> &sfml_events) {
  for (const auto &event : sfml_events) {
    if (event.is<sf::Event::Closed>()) {
      return true;
    }
    if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
      if (key_pressed->code == sf::Keyboard::Key::Escape) {
        return true;
      }
    }
  }
  return false;
}

} // namespace steamrot::events::convert
