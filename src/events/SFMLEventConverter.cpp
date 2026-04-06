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

  // Step 4: Convert window-close event to SystemPayload::QUIT.
  if (events::convert::HasWindowCloseEvent(sfml_events)) {
    auto packet =
        events::CreateSystemEventPacket(1, SystemPayload::SystemAction::QUIT);
    if (packet.has_value()) {
      result.push_back(packet.value());
    }
  }

  // Step 5: Convert window-resize event to SystemPayload::RESIZE.
  auto resize_size = events::convert::ExtractWindowResizeSize(sfml_events);
  if (resize_size.has_value()) {
    auto packet = events::CreateSystemEventPacket(
        1, SystemPayload::SystemAction::RESIZE, resize_size.value());
    if (packet.has_value()) {
      result.push_back(packet.value());
    }
  }

  // Step 6: Reset the waiting-room bitset for the next tick.
  m_waiting_room_bitset.reset();

  return result;
}

} // namespace steamrot
