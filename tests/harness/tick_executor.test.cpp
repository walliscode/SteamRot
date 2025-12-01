/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for tick executor functionality
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "tick_executor.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("determine_num_ticks uses explicit num_ticks from TestDataConfig",
          "[unit][harness][tick_executor]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create inputs with tick = 3, but set explicit num_ticks = 5
  auto pos0 = steamrot::CreateVector2fData(builder, 100.0f, 100.0f);
  auto mouse0 = steamrot::CreateMouseInputData(builder, pos0, 0);
  auto event0 = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse0.Union(), 1);

  auto pos1 = steamrot::CreateVector2fData(builder, 150.0f, 150.0f);
  auto mouse1 = steamrot::CreateMouseInputData(builder, pos1, 0);
  auto event1 = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse1.Union(), 3);

  std::vector<flatbuffers::Offset<steamrot::InputEvent>> events;
  events.push_back(event0);
  events.push_back(event1);

  auto events_vec = builder.CreateVector(events);
  auto input_seq = steamrot::CreateInputSequence(builder, events_vec);

  auto metadata =
      steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  // Explicitly set num_ticks = 5
  // Parameters: metadata, start_data_collection, expected_data_collection,
  //             game_resources, scene_resources, simulation_data,
  //             input_sequence, event_sequence, num_ticks
  auto config = steamrot::CreateTestDataConfig(builder, metadata, 0, 0, 0, 0, 0,
                                               input_seq, 0, 5);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      flatbuffers::GetRoot<steamrot::TestDataConfig>(
          builder.GetBufferPointer());

  // Should return exactly 5, not based on input_sequence max tick
  uint32_t num_ticks = steamrot::tests::DetermineNumTicks(test_config);
  REQUIRE(num_ticks == 5);
}

TEST_CASE("determine_num_ticks ignores event_sequence max tick",
          "[unit][harness][tick_executor]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create events with max tick = 2
  auto packet0 = steamrot::CreateEventPacketData(
      builder, 5, steamrot::EventType_EVENT_TEST);
  auto event0 = steamrot::CreateEventTestData(builder, 1, packet0);

  auto packet1 = steamrot::CreateEventPacketData(
      builder, 3, steamrot::EventType_EVENT_USER_INPUT);
  auto event1 = steamrot::CreateEventTestData(builder, 2, packet1);

  std::vector<flatbuffers::Offset<steamrot::EventTestData>> events;
  events.push_back(event0);
  events.push_back(event1);

  auto events_vec = builder.CreateVector(events);
  auto event_seq = steamrot::CreateEventSequence(builder, events_vec);

  auto metadata =
      steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  // No explicit num_ticks set, should default to 1 despite event_sequence
  // Parameters: metadata, start_data_collection, expected_data_collection,
  //             game_resources, scene_resources, simulation_data,
  //             input_sequence, event_sequence, num_ticks
  auto config = steamrot::CreateTestDataConfig(builder, metadata, 0, 0, 0, 0, 0,
                                               0, event_seq);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      flatbuffers::GetRoot<steamrot::TestDataConfig>(
          builder.GetBufferPointer());

  // Should be 1 (default), not based on event_sequence max tick
  uint32_t num_ticks = steamrot::tests::DetermineNumTicks(test_config);
  REQUIRE(num_ticks == 1);
}

TEST_CASE("determine_num_ticks defaults to 1 when num_ticks not specified",
          "[unit][harness][tick_executor]") {
  flatbuffers::FlatBufferBuilder builder;

  auto metadata =
      steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  // No num_ticks specified, should default to 1
  auto config = steamrot::CreateTestDataConfig(builder, metadata);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      flatbuffers::GetRoot<steamrot::TestDataConfig>(
          builder.GetBufferPointer());

  uint32_t num_ticks = steamrot::tests::DetermineNumTicks(test_config);
  REQUIRE(num_ticks == 1);
}

TEST_CASE("execute_single_tick handles null config",
          "[unit][harness][tick_executor]") {
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::ExecuteSingleTick(1, nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_tick_based_test handles null config",
          "[unit][harness][tick_executor]") {
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::ExecuteTickBasedTest(nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("determine_num_ticks ignores simulation_data num_ticks",
          "[unit][harness][tick_executor]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create simulation data with num_ticks = 10
  std::vector<flatbuffers::Offset<steamrot::SimulationStep>> steps;
  auto sim_data =
      steamrot::CreateSimulationData(builder, builder.CreateVector(steps), 0);

  auto metadata =
      steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  // TestDataConfig without explicit num_ticks (should default to 1, not 10)
  // Parameters: metadata, start_data_collection, expected_data_collection,
  //             game_resources, scene_resources, simulation_data,
  //             input_sequence, event_sequence, num_ticks
  auto config = steamrot::CreateTestDataConfig(builder, metadata, 0, 0, 0, 0,
                                               sim_data);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      flatbuffers::GetRoot<steamrot::TestDataConfig>(
          builder.GetBufferPointer());

  uint32_t num_ticks = steamrot::tests::DetermineNumTicks(test_config);
  // Should be 1 (default), not 10 from simulation_data
  REQUIRE(num_ticks == 1);
}

TEST_CASE("execute_tick_based_test executes specified num_ticks",
          "[unit][harness][tick_executor]") {
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  flatbuffers::FlatBufferBuilder builder;

  // Create input sequence with events on different ticks (1-based)
  auto pos0 = steamrot::CreateVector2fData(builder, 100.0f, 100.0f);
  auto mouse0 = steamrot::CreateMouseInputData(builder, pos0, 0);
  auto input0 = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse0.Union(), 1);

  auto pos1 = steamrot::CreateVector2fData(builder, 200.0f, 200.0f);
  auto mouse1 = steamrot::CreateMouseInputData(builder, pos1, 0);
  auto input1 = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse1.Union(), 2);

  std::vector<flatbuffers::Offset<steamrot::InputEvent>> inputs;
  inputs.push_back(input0);
  inputs.push_back(input1);

  auto inputs_vec = builder.CreateVector(inputs);
  auto input_seq = steamrot::CreateInputSequence(builder, inputs_vec);

  auto metadata =
      steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  // Explicitly set num_ticks = 2 to execute both input events
  // Parameters: metadata, start_data_collection, expected_data_collection,
  //             game_resources, scene_resources, simulation_data,
  //             input_sequence, event_sequence, num_ticks
  auto config = steamrot::CreateTestDataConfig(builder, metadata, 0, 0, 0, 0, 0,
                                               input_seq, 0, 2);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      flatbuffers::GetRoot<steamrot::TestDataConfig>(
          builder.GetBufferPointer());

  // Execute the test
  auto result = steamrot::tests::ExecuteTickBasedTest(test_config, fixture);
  REQUIRE(result.has_value());

  // Verify mouse position reflects the last tick (tick 2)
  auto &game_context = fixture.GetGameContext();
  REQUIRE(game_context.mouse_position.x == 200);
  REQUIRE(game_context.mouse_position.y == 200);
}
