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
#include "flatbuffers/flatbuffer_builder.h"
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

TEST_CASE("InjectEvent works with different event types",
          "[unit][harness][event_simulation]") {
  steamrot::EventHandler event_handler;

  SECTION("EVENT_USER_INPUT") {
    flatbuffers::FlatBufferBuilder builder;
    auto event_packet = steamrot::CreateEventPacketData(
        builder, 3, steamrot::EventType_EVENT_USER_INPUT);
    builder.Finish(event_packet);

    const steamrot::EventPacketData *event_data =
        flatbuffers::GetRoot<steamrot::EventPacketData>(
            builder.GetBufferPointer());

    auto result = steamrot::tests::InjectEvent(event_data, event_handler);
    REQUIRE(result.has_value());

    event_handler.ProcessWaitingRoomEventBus();

    const auto &bus = event_handler.GetGlobalEventBus();
    REQUIRE(bus.size() == 1);
    REQUIRE(bus.back().m_event_type == steamrot::EventType_EVENT_USER_INPUT);
    REQUIRE(bus.back().event_lifetime == 3);
  }

  SECTION("EVENT_QUIT_GAME") {
    flatbuffers::FlatBufferBuilder builder;
    auto event_packet = steamrot::CreateEventPacketData(
        builder, 1, steamrot::EventType_EVENT_QUIT_GAME);
    builder.Finish(event_packet);

    const steamrot::EventPacketData *event_data =
        flatbuffers::GetRoot<steamrot::EventPacketData>(
            builder.GetBufferPointer());

    auto result = steamrot::tests::InjectEvent(event_data, event_handler);
    REQUIRE(result.has_value());

    event_handler.ProcessWaitingRoomEventBus();

    const auto &bus = event_handler.GetGlobalEventBus();
    REQUIRE(bus.size() == 1);
    REQUIRE(bus.back().m_event_type == steamrot::EventType_EVENT_QUIT_GAME);
    REQUIRE(bus.back().event_lifetime == 1);
  }
}
