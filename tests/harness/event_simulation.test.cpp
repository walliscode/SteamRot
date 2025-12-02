/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for event simulation functionality
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_simulation.h"
#include "EventHandler.h"
#include "event_packet_data_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("InjectEvent handles null event data",
          "[unit][harness][event_simulation]") {
  steamrot::EventHandler event_handler;

  auto result = steamrot::tests::InjectEvent(nullptr, event_handler);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("InjectEventsForTick handles null events vector",
          "[unit][harness][event_simulation]") {
  steamrot::EventHandler event_handler;

  auto result = steamrot::tests::InjectEventsForTick(nullptr, event_handler);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("InjectEvent adds event to waiting room",
          "[unit][harness][event_simulation]") {
  steamrot::EventHandler event_handler;

  // Create event packet data with a simple event
  flatbuffers::FlatBufferBuilder builder;
  auto event_packet = steamrot::CreateEventPacketData(
      builder, 5, steamrot::EventType_EVENT_TEST);
  builder.Finish(event_packet);

  const steamrot::EventPacketData *event_data =
      flatbuffers::GetRoot<steamrot::EventPacketData>(
          builder.GetBufferPointer());

  // Get initial event bus state
  const auto &initial_bus = event_handler.GetGlobalEventBus();
  size_t initial_size = initial_bus.size();

  // Execute the event
  auto result = steamrot::tests::InjectEvent(event_data, event_handler);
  REQUIRE(result.has_value());

  // Process waiting room to move event to global bus
  event_handler.ProcessWaitingRoomEventBus();

  // Verify event was added
  const auto &updated_bus = event_handler.GetGlobalEventBus();
  REQUIRE(updated_bus.size() == initial_size + 1);

  // Verify the event has correct type
  const auto &last_event = updated_bus.back();
  REQUIRE(last_event.m_event_type == steamrot::EventType_EVENT_TEST);
  REQUIRE(last_event.event_lifetime == 5);
}

TEST_CASE("InjectEventsForTick processes all events in vector",
          "[unit][harness][event_simulation]") {
  steamrot::EventHandler event_handler;

  // Create event packet data vector with multiple events
  flatbuffers::FlatBufferBuilder builder;

  auto packet0 = steamrot::CreateEventPacketData(
      builder, 5, steamrot::EventType_EVENT_TEST);
  auto packet1 = steamrot::CreateEventPacketData(
      builder, 3, steamrot::EventType_EVENT_USER_INPUT);
  auto packet2 = steamrot::CreateEventPacketData(
      builder, 1, steamrot::EventType_EVENT_QUIT_GAME);

  std::vector<flatbuffers::Offset<steamrot::EventPacketData>> events;
  events.push_back(packet0);
  events.push_back(packet1);
  events.push_back(packet2);

  auto events_vec = builder.CreateVector(events);
  builder.Finish(events_vec);

  // Get the vector from the buffer
  const auto *event_vector =
      flatbuffers::GetRoot<flatbuffers::Vector<
          flatbuffers::Offset<steamrot::EventPacketData>>>(
          builder.GetBufferPointer());

  // Cast to the expected type
  const flatbuffers::Vector<steamrot::EventPacketData *> *typed_vector =
      reinterpret_cast<
          const flatbuffers::Vector<steamrot::EventPacketData *> *>(
          event_vector);

  // Execute all events
  auto result =
      steamrot::tests::InjectEventsForTick(typed_vector, event_handler);
  REQUIRE(result.has_value());

  // Process waiting room to move all events to global bus
  event_handler.ProcessWaitingRoomEventBus();

  // Verify all events were added
  const auto &bus = event_handler.GetGlobalEventBus();
  REQUIRE(bus.size() == 3);

  // Verify events in order
  REQUIRE(bus[0].m_event_type == steamrot::EventType_EVENT_TEST);
  REQUIRE(bus[1].m_event_type == steamrot::EventType_EVENT_USER_INPUT);
  REQUIRE(bus[2].m_event_type == steamrot::EventType_EVENT_QUIT_GAME);
}
