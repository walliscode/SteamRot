/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for converting SFML events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "sfml_event_convert.h"
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

  // Iterate the registry in order — first match wins.
  for (const auto &[pattern, action] : registry) {

    // Cast both sides to the base bitset so operator& is available.
    const auto &pat =
        static_cast<const std::bitset<kTotalBits> &>(pattern);
    const auto &acc =
        static_cast<const std::bitset<kTotalBits> &>(accumulated);

    // The pattern matches when every bit required by the pattern is
    // set in the accumulated bitset (subset / "contains" check).
    if ((pat & acc) == pat && pat.any()) {
      return action;
    }
  }

  return std::nullopt;
}

} // namespace steamrot::events::convert
