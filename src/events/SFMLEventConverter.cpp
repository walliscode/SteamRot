/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SFMLEventConverter class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SFMLEventConverter.h"
#include "EventPayload.h"
#include "event_factory.h"
#include "sfml_event_convert.h"

namespace steamrot {

/////////////////////////////////////////////////
void SFMLEventConverter::SetInputActionRegistry(InputActionRegistry &&registry) {
  m_input_action_registry = std::move(registry);
}

/////////////////////////////////////////////////
std::vector<EventPacket>
SFMLEventConverter::ConvertSFMLEvents(const std::vector<sf::Event> &sfml_events) {
  std::vector<EventPacket> result;

  // Step 1: Collect keyboard/mouse SFML events into the waiting-room bitset.
  m_waiting_room_bitset = events::convert::CollectInputEvents(sfml_events);

  // Step 2: Resolve an InputAction from the registry.
  auto input_action =
      events::convert::ResolveInputAction(m_waiting_room_bitset,
                                          m_input_action_registry);

  // Step 3: Create an EventPacket for the resolved action (if any).
  if (input_action.has_value()) {
    auto packet = events::CreateInputEventPacket(1, input_action.value());
    if (packet.has_value()) {
      result.push_back(packet.value());
    }
  }

  // Step 4: Reset the waiting-room bitset for the next tick.
  m_waiting_room_bitset.reset();

  // Step 5: Collect system events (window close / Escape key).
  bool should_quit = events::convert::CollectSystemEvents(sfml_events);

  // Step 6: Emit a SYSTEM QUIT EventPacket if a quit event was detected.
  if (should_quit) {
    // lifetime of 1 tick — consistent with all other single-frame events
    auto quit_packet = events::CreateSystemEventPacket(
        1, SystemPayload::SystemAction::QUIT);
    if (quit_packet.has_value()) {
      result.push_back(quit_packet.value());
    }
  }

  return result;
}

} // namespace steamrot
