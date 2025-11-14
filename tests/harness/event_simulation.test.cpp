/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for event simulation functionality
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_simulation.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("execute_event_test_data handles null event data",
          "[unit][harness][event_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::ExecuteEventTestData(nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_events_for_tick handles null sequence",
          "[unit][harness][event_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();


  auto result = steamrot::tests::ExecuteEventsForTick(nullptr, 0, fixture);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_event_sequence handles null sequence",
          "[unit][harness][event_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::ExecuteEventSequence(nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_event_sequence handles empty sequence",
          "[unit][harness][event_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create empty event sequence
  flatbuffers::FlatBufferBuilder builder;
  auto sequence = steamrot::CreateEventSequence(builder);
  builder.Finish(sequence);

  const steamrot::EventSequence *event_sequence =
      flatbuffers::GetRoot<steamrot::EventSequence>(builder.GetBufferPointer());

  auto result =
      steamrot::tests::ExecuteEventSequence(event_sequence, fixture);
  REQUIRE(result.has_value());
}

TEST_CASE("execute_event_test_data adds event to waiting room",
          "[unit][harness][event_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create event test data with a simple event
  flatbuffers::FlatBufferBuilder builder;
  auto event_packet = steamrot::CreateEventPacketData(
      builder, 5, steamrot::EventType_EVENT_TEST);
  auto event_data = steamrot::CreateEventTestData(builder, 0, event_packet);
  builder.Finish(event_data);

  const steamrot::EventTestData *test_data =
      flatbuffers::GetRoot<steamrot::EventTestData>(builder.GetBufferPointer());

  // Get initial event bus state
  auto &event_handler = fixture.GetGameResources().event_handler;
  const auto &initial_bus = event_handler.GetGlobalEventBus();
  size_t initial_size = initial_bus.size();

  // Execute the event
  auto result = steamrot::tests::ExecuteEventTestData(test_data, fixture);
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

TEST_CASE("execute_events_for_tick processes only specified tick",
          "[unit][harness][event_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create event sequence with events on different ticks
  flatbuffers::FlatBufferBuilder builder;

  // Event at tick 1
  auto packet0 = steamrot::CreateEventPacketData(
      builder, 5, steamrot::EventType_EVENT_TEST);
  auto event0 = steamrot::CreateEventTestData(builder, 1, packet0);

  // Event at tick 2
  auto packet1 = steamrot::CreateEventPacketData(
      builder, 5, steamrot::EventType_EVENT_USER_INPUT);
  auto event1 = steamrot::CreateEventTestData(builder, 2, packet1);

  std::vector<flatbuffers::Offset<steamrot::EventTestData>> events;
  events.push_back(event0);
  events.push_back(event1);

  auto events_vec = builder.CreateVector(events);
  auto sequence = steamrot::CreateEventSequence(builder, events_vec);
  builder.Finish(sequence);

  const steamrot::EventSequence *event_sequence =
      flatbuffers::GetRoot<steamrot::EventSequence>(builder.GetBufferPointer());

  auto &event_handler = fixture.GetGameResources().event_handler;

  // Execute only tick 1
  auto result =
      steamrot::tests::ExecuteEventsForTick(event_sequence, 0, fixture);
  REQUIRE(result.has_value());

  // Process waiting room
  event_handler.ProcessWaitingRoomEventBus();

  // Verify only one event was added
  const auto &bus_after_tick1 = event_handler.GetGlobalEventBus();
  size_t size_after_tick1 = bus_after_tick1.size();
  REQUIRE(size_after_tick1 == 1);
  REQUIRE(bus_after_tick1.back().m_event_type ==
          steamrot::EventType_EVENT_TEST);

  // Execute tick 1
  result = steamrot::tests::ExecuteEventsForTick(event_sequence, 1, fixture);
  REQUIRE(result.has_value());

  // Process waiting room
  event_handler.ProcessWaitingRoomEventBus();

  // Verify second event was added
  const auto &bus_after_tick2 = event_handler.GetGlobalEventBus();
  REQUIRE(bus_after_tick2.size() == size_after_tick1 + 1);
  REQUIRE(bus_after_tick2.back().m_event_type ==
          steamrot::EventType_EVENT_USER_INPUT);
}

TEST_CASE("execute_event_sequence processes all ticks in order",
          "[unit][harness][event_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create event sequence with multiple events
  flatbuffers::FlatBufferBuilder builder;

  auto packet0 = steamrot::CreateEventPacketData(
      builder, 5, steamrot::EventType_EVENT_TEST);
  auto event0 = steamrot::CreateEventTestData(builder, 1, packet0);

  auto packet1 = steamrot::CreateEventPacketData(
      builder, 3, steamrot::EventType_EVENT_USER_INPUT);
  auto event1 = steamrot::CreateEventTestData(builder, 3, packet1);

  auto packet2 = steamrot::CreateEventPacketData(
      builder, 1, steamrot::EventType_EVENT_QUIT_GAME);
  auto event2 = steamrot::CreateEventTestData(builder, 2, packet2);

  std::vector<flatbuffers::Offset<steamrot::EventTestData>> events;
  events.push_back(event0);
  events.push_back(event1);
  events.push_back(event2);

  auto events_vec = builder.CreateVector(events);
  auto sequence = steamrot::CreateEventSequence(builder, events_vec);
  builder.Finish(sequence);

  const steamrot::EventSequence *event_sequence =
      flatbuffers::GetRoot<steamrot::EventSequence>(builder.GetBufferPointer());

  // Execute entire sequence
  auto result =
      steamrot::tests::ExecuteEventSequence(event_sequence, fixture);
  REQUIRE(result.has_value());

  // Process waiting room to move all events to global bus
  auto &event_handler = fixture.GetGameResources().event_handler;
  event_handler.ProcessWaitingRoomEventBus();

  // Verify all events were added
  const auto &bus = event_handler.GetGlobalEventBus();
  REQUIRE(bus.size() == 3);

  // Events should be processed in tick order (1, 2, 3)
  REQUIRE(bus[0].m_event_type == steamrot::EventType_EVENT_TEST);
  REQUIRE(bus[1].m_event_type == steamrot::EventType_EVENT_QUIT_GAME);
  REQUIRE(bus[2].m_event_type == steamrot::EventType_EVENT_USER_INPUT);
}
