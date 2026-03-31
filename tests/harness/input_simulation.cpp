/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to simulate input events for testing
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "input_simulation.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_events_for_tick(const std::vector<sf::Event> &sfml_events,
                              EventHandler &event_handler) {
  // Feed through the existing SFML event conversion pipeline so that
  // bitset accumulation, input-action resolution and EventPacket creation
  // all happen exactly as they would with real hardware events.
  event_handler.ConvertSFMLEventsToEventPackets(sfml_events);
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_sequence(
    const std::unordered_map<size_t, std::vector<sf::Event>>
        &input_events_by_tick,
    size_t current_tick, EventHandler &event_handler) {

  const auto it = input_events_by_tick.find(current_tick);
  if (it == input_events_by_tick.end()) {
    return std::monostate{};
  }

  return execute_input_events_for_tick(it->second, event_handler);
}

} // namespace steamrot::tests
