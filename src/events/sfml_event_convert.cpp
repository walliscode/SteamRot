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

} // namespace steamrot::events::convert
