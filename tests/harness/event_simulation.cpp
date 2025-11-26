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
#include "events_generated.h"
#include <iostream>
#include <set>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteEventTestData(const EventTestData *event_data, TestFixture &fixture) {

  std::cout << "Executing EventTestData for tick " << event_data->tick()
            << std::endl;
  // Validate event data
  if (!event_data) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EventTestData is null"));
  }

  if (!event_data->event_packet()) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EventPacketData is null"));
  }

  // Create EventPacket from the data using factory function
  auto event_packet_result =
      event::CreateEventPacketFromData(event_data->event_packet());

  if (!event_packet_result.has_value()) {
    return std::unexpected(event_packet_result.error());
  }

  EventPacket event_packet = event_packet_result.value();
  std::cout << "Created EventPacket with type "
            << EnumNameEventType(event_packet.m_event_type) << std::endl;

  // Add event to the event handler
  EventHandler &event_handler = fixture.GetGameResources().event_handler;
  event_handler.AddEvent(event_packet);
  std::cout << "Added Event of type "
            << EnumNameEventType(event_packet.m_event_type)
            << " to EventHandler" << std::endl;

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteEventsForTick(const EventSequence *event_sequence, uint32_t tick,
                     TestFixture &fixture) {

  // Validate event sequence
  if (!event_sequence) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EventSequence is null"));
  }

  if (!event_sequence->events()) {
    // No events to process - not an error
    return std::monostate{};
  }

  // Process all events for this tick
  for (const EventTestData *event_data : *event_sequence->events()) {
    if (!event_data) {
      continue; // Skip null events
    }

    if (event_data->tick() == tick) {
      auto result = ExecuteEventTestData(event_data, fixture);
      if (!result.has_value()) {
        return std::unexpected(result.error());
      }
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteEventSequence(const EventSequence *event_sequence,
                     TestFixture &fixture) {

  // Validate event sequence
  if (!event_sequence) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EventSequence is null"));
  }

  if (!event_sequence->events() || event_sequence->events()->size() == 0) {
    // No events to process - not an error
    return std::monostate{};
  }

  // Collect all unique tick values
  std::set<uint32_t> ticks;
  for (const EventTestData *event_data : *event_sequence->events()) {
    if (event_data) {
      ticks.insert(event_data->tick());
    }
  }

  // Process events for each tick in order
  for (uint32_t tick : ticks) {
    auto result = ExecuteEventsForTick(event_sequence, tick, fixture);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }

  return std::monostate{};
}

} // namespace steamrot::tests
