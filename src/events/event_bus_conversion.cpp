/////////////////////////////////////////////////
/// @file
/// @brief Implementation of EventBus conversion utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_bus_conversion.h"
#include "event_factory.h"

namespace steamrot::event {

/////////////////////////////////////////////////
std::expected<EventBus, FailInfo>
ConvertEventBusDataToEventBus(const EventBusData *event_bus_data) {

  // Validate input
  if (!event_bus_data) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EventBusData is null"));
  }

  EventBus event_bus;

  // Early return if no events
  if (!event_bus_data->events() || event_bus_data->events()->size() == 0) {
    return event_bus; // Empty event bus
  }

  // Convert each EventPacketData to EventPacket
  for (const EventPacketData *packet_data : *event_bus_data->events()) {
    if (!packet_data) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "EventPacketData is null"));
    }

    // Create EventPacket from the data
    EventPacket event_packet(packet_data->event_lifetime());
    event_packet.m_event_type = packet_data->event_type();

    // set EventData
    auto event_data_creation_result = event::CreateEventData(
        packet_data->event_data_data_type(), packet_data->event_data_data());
    // Check for errors during EventData creation, if not, set the EventData
    if (!event_data_creation_result.has_value()) {
      return std::unexpected(event_data_creation_result.error());
    } else {
      event_packet.m_event_data = event_data_creation_result.value();
    }
    event_bus.push_back(event_packet);
  }

  return event_bus;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventHandlerFromEventBusData(const EventBusData *event_bus_data,
                                      EventHandler &event_handler) {

  // Convert EventBusData to EventBus
  auto event_bus_result = ConvertEventBusDataToEventBus(event_bus_data);

  if (!event_bus_result.has_value()) {
    return std::unexpected(event_bus_result.error());
  }

  EventBus event_bus = event_bus_result.value();

  // The EventHandler doesn't have a public method to directly set the global
  // event bus, so we need to add events individually using AddEvent and then
  // process the waiting room.
  //
  // However, this won't preserve the exact state of the global event bus
  // because AddEvent adds to the waiting room, not the global bus.
  //
  // For now, we'll add each event to the event handler's waiting room
  // and then process it. The caller should call ProcessWaitingRoomEventBus()
  // after this function to move events to the global bus.

  for (const EventPacket &event : event_bus) {
    event_handler.AddEvent(event);
  }

  // Process the waiting room to move events to the global event bus
  event_handler.ProcessWaitingRoomEventBus();

  return std::monostate{};
}

} // namespace steamrot::event
