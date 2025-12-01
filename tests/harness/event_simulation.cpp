/////////////////////////////////////////////////
/// @file
/// @brief Implementation of event simulation utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_simulation.h"
#include "EventPacket.h"
#include "event_factory.h"
#include "event_packet_data_generated.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InjectEvent(const EventPacketData *event_packet_data,
            EventHandler &event_handler) {

  // Validate event data
  if (!event_packet_data) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EventPacketData is null"));
  }

  // Create EventPacket from the data using factory function
  auto event_packet_result =
      event::CreateEventPacketFromData(event_packet_data);

  if (!event_packet_result.has_value()) {
    return std::unexpected(event_packet_result.error());
  }

  EventPacket event_packet = event_packet_result.value();

  event_handler.AddEvent(event_packet);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InjectAllEventsForTick(const flatbuffers::Vector<EventPacketData *> *events,
                       EventHandler &event_handler) {

  if (!events) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "Events vector is null"));
  }

  // Process all events for this tick
  for (const EventPacketData *event_data : *events) {
    if (!event_data) {
      continue; // Skip null events
    }

    auto result = InjectEvent(event_data, event_handler);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }

  return std::monostate{};
}
} // namespace steamrot::tests
