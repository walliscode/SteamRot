/////////////////////////////////////////////////
/// @file
/// @brief Implementation of event simulation utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_simulation.h"
#include "EventPacket.h"
#include "SceneChangePacket.h"
#include "UserInputBitset.h"
#include <algorithm>
#include <format>
#include <set>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_event_test_data(const EventTestData *event_data, TestFixture &fixture) {

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
  EventPacket event_packet;
  event_packet.event_lifetime = packet_data->event_lifetime();
  event_packet.event_type = packet_data->event_type();

  // Handle different event data types
  if (packet_data->event_data_data_type() ==
      EventDataData_UserInputBitsetData) {
    const UserInputBitsetData *input_data =
        packet_data->event_data_data_as_UserInputBitsetData();
    if (input_data) {
      // Create UserInputBitset from the data
      UserInputBitset input_bitset;

      // Process keyboard pressed
      if (input_data->keyboard_pressed()) {
        for (auto key : *input_data->keyboard_pressed()) {
          input_bitset.SetKeyboard(key, true);
        }
      }

      // Process keyboard released
      if (input_data->keyboard_released()) {
        for (auto key : *input_data->keyboard_released()) {
          input_bitset.SetKeyboard(key, false);
        }
      }

      // Process mouse pressed
      if (input_data->mouse_pressed()) {
        for (auto button : *input_data->mouse_pressed()) {
          input_bitset.SetMouse(button, true);
        }
      }

      // Process mouse released
      if (input_data->mouse_released()) {
        for (auto button : *input_data->mouse_released()) {
          input_bitset.SetMouse(button, false);
        }
      }

      event_packet.event_data = input_bitset;
    }
  } else if (packet_data->event_data_data_type() ==
             EventDataData_SceneChangePacketData) {
    const SceneChangePacketData *scene_data =
        packet_data->event_data_data_as_SceneChangePacketData();
    if (scene_data) {
      SceneChangePacket scene_packet;
      scene_packet.scene_type = scene_data->scene_type();
      event_packet.event_data = scene_packet;
    }
  }

  // Add event to the event handler
  EventHandler &event_handler = fixture.GetGameResources().event_handler;
  event_handler.AddEvent(event_packet);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_events_for_tick(const EventSequence *event_sequence, uint32_t tick,
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
      auto result = execute_event_test_data(event_data, fixture);
      if (!result.has_value()) {
        return std::unexpected(result.error());
      }
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_event_sequence(const EventSequence *event_sequence,
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
    auto result = execute_events_for_tick(event_sequence, tick, fixture);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }

  return std::monostate{};
}

} // namespace steamrot::tests
