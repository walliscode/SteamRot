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

TEST_CASE("determine_num_ticks with explicit num_ticks",
          "[unit][harness][tick_executor]") {
  flatbuffers::FlatBufferBuilder builder;

  auto metadata = steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  auto config = steamrot::CreateTestDataConfig(builder, metadata, 0, 0, 0, 0, 0, 0, 0, 5);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      steamrot::GetTestDataConfig(builder.GetBufferPointer());

  uint32_t num_ticks = steamrot::tests::determine_num_ticks(test_config);
  REQUIRE(num_ticks == 5);
}

TEST_CASE("determine_num_ticks auto-detects from input sequence",
          "[unit][harness][tick_executor]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create inputs with max tick = 3
  auto pos0 = steamrot::CreateVector2fData(builder, 100.0f, 100.0f);
  auto mouse0 = steamrot::CreateMouseInputData(builder, pos0, 0);
  auto event0 = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse0.Union(), 0);

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

  auto metadata = steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  auto config = steamrot::CreateTestDataConfig(builder, metadata, 0, 0, 0, 0, 0,
                                                input_seq);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      steamrot::GetTestDataConfig(builder.GetBufferPointer());

  // Max tick is 3, so should return 4 (ticks 0,1,2,3)
  uint32_t num_ticks = steamrot::tests::determine_num_ticks(test_config);
  REQUIRE(num_ticks == 4);
}

TEST_CASE("determine_num_ticks auto-detects from event sequence",
          "[unit][harness][tick_executor]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create events with max tick = 2
  auto packet0 = steamrot::CreateEventPacketData(
      builder, 5, steamrot::EventType_EVENT_TEST);
  auto event0 = steamrot::CreateEventTestData(builder, 0, packet0);

  auto packet1 = steamrot::CreateEventPacketData(
      builder, 3, steamrot::EventType_EVENT_USER_INPUT);
  auto event1 = steamrot::CreateEventTestData(builder, 2, packet1);

  std::vector<flatbuffers::Offset<steamrot::EventTestData>> events;
  events.push_back(event0);
  events.push_back(event1);

  auto events_vec = builder.CreateVector(events);
  auto event_seq = steamrot::CreateEventSequence(builder, events_vec);

  auto metadata = steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  auto config = steamrot::CreateTestDataConfig(builder, metadata, 0, 0, 0, 0, 0,
                                                0, event_seq);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      steamrot::GetTestDataConfig(builder.GetBufferPointer());

  // Max tick is 2, so should return 3 (ticks 0,1,2)
  uint32_t num_ticks = steamrot::tests::determine_num_ticks(test_config);
  REQUIRE(num_ticks == 3);
}

TEST_CASE("determine_num_ticks defaults to 1 when no sequences",
          "[unit][harness][tick_executor]") {
  flatbuffers::FlatBufferBuilder builder;

  auto metadata = steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  auto config = steamrot::CreateTestDataConfig(builder, metadata);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      steamrot::GetTestDataConfig(builder.GetBufferPointer());

  uint32_t num_ticks = steamrot::tests::determine_num_ticks(test_config);
  REQUIRE(num_ticks == 1);
}

TEST_CASE("execute_single_tick handles null config",
          "[unit][harness][tick_executor]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::execute_single_tick(0, nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_tick_based_test handles null config",
          "[unit][harness][tick_executor]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::execute_tick_based_test(nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_tick_based_test executes multiple ticks",
          "[unit][harness][tick_executor]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  flatbuffers::FlatBufferBuilder builder;

  // Create input sequence with events on different ticks
  auto pos0 = steamrot::CreateVector2fData(builder, 100.0f, 100.0f);
  auto mouse0 = steamrot::CreateMouseInputData(builder, pos0, 0);
  auto input0 = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse0.Union(), 0);

  auto pos1 = steamrot::CreateVector2fData(builder, 200.0f, 200.0f);
  auto mouse1 = steamrot::CreateMouseInputData(builder, pos1, 0);
  auto input1 = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse1.Union(), 1);

  std::vector<flatbuffers::Offset<steamrot::InputEvent>> inputs;
  inputs.push_back(input0);
  inputs.push_back(input1);

  auto inputs_vec = builder.CreateVector(inputs);
  auto input_seq = steamrot::CreateInputSequence(builder, inputs_vec);

  auto metadata = steamrot::CreateTestMetadata(builder, builder.CreateString("test"));
  auto config = steamrot::CreateTestDataConfig(builder, metadata, 0, 0, 0, 0, 0,
                                                input_seq);
  builder.Finish(config);

  const steamrot::TestDataConfig *test_config =
      steamrot::GetTestDataConfig(builder.GetBufferPointer());

  // Execute the test
  auto result = steamrot::tests::execute_tick_based_test(test_config, fixture);
  REQUIRE(result.has_value());

  // Verify mouse position reflects the last tick (tick 1)
  auto &game_context = fixture.GetGameContext();
  REQUIRE(game_context.mouse_position.x == 200);
  REQUIRE(game_context.mouse_position.y == 200);
}
