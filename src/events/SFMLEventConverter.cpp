/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SFMLEventConverter class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SFMLEventConverter.h"
#include "event_factory.h"
#include "sfml_event_convert.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SFMLEventConverter::Configure(const InputActionConfigFbs *config_data) {
  // Clear any previously loaded registry before repopulating.
  m_input_action_registry.clear();

  // Delegate to the free function which handles all FlatBuffers→registry logic.
  return events::convert::ConfigureInputActionRegistry(m_input_action_registry,
                                                        config_data);
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

  return result;
}

} // namespace steamrot
