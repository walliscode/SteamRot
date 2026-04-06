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
#include <SFML/System/Vector2.hpp>
#include <bitset>
#include <optional>

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
bool HasWindowCloseEvent(const std::vector<sf::Event> &sfml_events) {
  for (const auto &event : sfml_events) {
    if (event.getIf<sf::Event::Closed>()) {
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////
std::optional<sf::Vector2u>
ExtractWindowResizeSize(const std::vector<sf::Event> &sfml_events) {
  for (const auto &event : sfml_events) {
    if (const auto *resized = event.getIf<sf::Event::Resized>()) {
      return resized->size;
    }
  }
  return std::nullopt;
}

} // namespace steamrot::events::convert
