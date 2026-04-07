/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for converting SFML events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "sfml_event_convert.h"
#include "EventContext.h"
#include "EventType.h"
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
std::optional<EventPacket>
ConvertResizeEvents(const std::vector<sf::Event> &sfml_events) {
  for (const auto &event : sfml_events) {
    if (const auto *resized = event.getIf<sf::Event::Resized>()) {
      EventContext context{1};
      SystemPayload payload{SystemPayload::SystemAction::RESIZE,
                            resized->size};
      return EventPacket{context, EventType::SYSTEM, payload};
    }
  }
  return std::nullopt;
}

} // namespace steamrot::events::convert
