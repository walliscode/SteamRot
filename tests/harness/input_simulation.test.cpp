/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for input simulation functionality
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "input_simulation.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("execute_input_event handles null input", "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::execute_input_event(nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_input_events_for_tick handles null sequence",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::execute_input_events_for_tick(nullptr, 0, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_input_sequence handles null sequence",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::execute_input_sequence(nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_input_sequence handles empty sequence",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create empty input sequence
  flatbuffers::FlatBufferBuilder builder;
  auto sequence = steamrot::CreateInputSequence(builder);
  builder.Finish(sequence);

  const steamrot::InputSequence *input_sequence =
      steamrot::GetInputSequence(builder.GetBufferPointer());

  auto result = steamrot::tests::execute_input_sequence(input_sequence, fixture);
  REQUIRE(result.has_value());
}

TEST_CASE("execute_input_event updates mouse position for MouseMove",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create mouse move event
  flatbuffers::FlatBufferBuilder builder;
  auto position = steamrot::CreateVector2fData(builder, 150.0f, 200.0f);
  auto mouse_data = steamrot::CreateMouseInputData(builder, position, 0);
  auto input_event = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse_data.Union(), 0);
  builder.Finish(input_event);

  const steamrot::InputEvent *event =
      steamrot::GetInputEvent(builder.GetBufferPointer());

  // Execute the event
  auto result = steamrot::tests::execute_input_event(event, fixture);
  REQUIRE(result.has_value());

  // Verify mouse position was updated
  auto &game_context = fixture.GetGameContext();
  REQUIRE(game_context.mouse_position.x == 150);
  REQUIRE(game_context.mouse_position.y == 200);
}

TEST_CASE("execute_input_events_for_tick processes only specified tick",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create input sequence with events on different ticks
  flatbuffers::FlatBufferBuilder builder;

  // Event at tick 0
  auto pos0 = steamrot::CreateVector2fData(builder, 100.0f, 100.0f);
  auto mouse0 = steamrot::CreateMouseInputData(builder, pos0, 0);
  auto event0 = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse0.Union(), 0);

  // Event at tick 1
  auto pos1 = steamrot::CreateVector2fData(builder, 200.0f, 200.0f);
  auto mouse1 = steamrot::CreateMouseInputData(builder, pos1, 0);
  auto event1 = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse1.Union(), 1);

  std::vector<flatbuffers::Offset<steamrot::InputEvent>> events;
  events.push_back(event0);
  events.push_back(event1);

  auto events_vec = builder.CreateVector(events);
  auto sequence = steamrot::CreateInputSequence(builder, events_vec);
  builder.Finish(sequence);

  const steamrot::InputSequence *input_sequence =
      steamrot::GetInputSequence(builder.GetBufferPointer());

  // Execute only tick 0
  auto result =
      steamrot::tests::execute_input_events_for_tick(input_sequence, 0, fixture);
  REQUIRE(result.has_value());

  // Verify only tick 0 event was processed
  auto &game_context = fixture.GetGameContext();
  REQUIRE(game_context.mouse_position.x == 100);
  REQUIRE(game_context.mouse_position.y == 100);

  // Execute tick 1
  result = steamrot::tests::execute_input_events_for_tick(input_sequence, 1, fixture);
  REQUIRE(result.has_value());

  // Verify tick 1 event was processed
  REQUIRE(game_context.mouse_position.x == 200);
  REQUIRE(game_context.mouse_position.y == 200);
}
