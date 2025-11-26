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
#include "uuid.h"
#include <iostream>
#include <set>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteEventTestData(const EventTestData *event_data, TestFixture &fixture) {

  // Validate event data
  if (!event_data) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EventTestData is null"));
  }

  if (!event_data->event_packet()) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EventPacketData is null"));
  }

  const EventPacketData *packet_data = event_data->event_packet();

  // Create EventPacket from the data
  EventPacket event_packet(packet_data->event_lifetime());
  event_packet.m_event_type = packet_data->event_type();

  // Handle different event data types
  if (packet_data->event_data_data_type() ==
      EventDataData_UserInputBitsetData) {
    const UserInputBitsetData *input_data =
        packet_data->event_data_data_as_UserInputBitsetData();
    if (input_data) {
      // Use the existing factory function from event_factory.h
      auto input_bitset_result = event::CreateUserInputBitset(*input_data);

      if (!input_bitset_result.has_value()) {
        return std::unexpected(input_bitset_result.error());
      }

      event_packet.m_event_data = input_bitset_result.value();
    }
  } else if (packet_data->event_data_data_type() ==
             EventDataData_SceneChangePacketData) {
    const SceneChangePacketData *scene_data =
        packet_data->event_data_data_as_SceneChangePacketData();
    if (scene_data) {
      // SceneChangePacket is std::pair<std::optional<uuids::uuid>, SceneType>
      std::optional<uuids::uuid> uuid_opt;
      if (scene_data->uuid()) {
        // Parse UUID string if provided
        std::string uuid_str = scene_data->uuid()->str();
        if (uuids::uuid::is_valid_uuid(uuid_str.c_str())) {
          uuid_opt = uuids::uuid::from_string(uuid_str.c_str());
        }
      }
      SceneChangePacket scene_packet =
          std::make_pair(uuid_opt, scene_data->scene_type());
      event_packet.m_event_data = scene_packet;
    }
  }

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
