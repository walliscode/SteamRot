/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for input simulation functionality
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "input_simulation.h"
#include "TestFixture.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("execute_input_event handles null input",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::execute_input_event(nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_input_events_for_tick handles null sequence",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result =
      steamrot::tests::execute_input_events_for_tick(nullptr, 0, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_input_sequence handles null sequence",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  auto result = steamrot::tests::execute_input_sequence(nullptr, fixture);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
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
      flatbuffers::GetRoot<steamrot::InputSequence>(builder.GetBufferPointer());

  auto result =
      steamrot::tests::execute_input_sequence(input_sequence, fixture);
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
      flatbuffers::GetRoot<steamrot::InputEvent>(builder.GetBufferPointer());

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
      flatbuffers::GetRoot<steamrot::InputSequence>(builder.GetBufferPointer());

  // Execute only tick 0
  auto result = steamrot::tests::execute_input_events_for_tick(input_sequence,
                                                               0, fixture);
  REQUIRE(result.has_value());

  // Verify only tick 0 event was processed
  auto &game_context = fixture.GetGameContext();
  REQUIRE(game_context.mouse_position.x == 100);
  REQUIRE(game_context.mouse_position.y == 100);

  // Execute tick 1
  result = steamrot::tests::execute_input_events_for_tick(input_sequence, 1,
                                                          fixture);
  REQUIRE(result.has_value());

  // Verify tick 1 event was processed
  REQUIRE(game_context.mouse_position.x == 200);
  REQUIRE(game_context.mouse_position.y == 200);
}

TEST_CASE("execute_input_event generates EventPacket for MouseClick",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create mouse click event
  flatbuffers::FlatBufferBuilder builder;
  auto position = steamrot::CreateVector2fData(builder, 100.0f, 150.0f);
  auto mouse_data = steamrot::CreateMouseInputData(
      builder, position, static_cast<uint8_t>(sf::Mouse::Button::Left));
  auto input_event = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseClick,
      steamrot::InputEventData_MouseInputData, mouse_data.Union(), 0);
  builder.Finish(input_event);

  const steamrot::InputEvent *event =
      flatbuffers::GetRoot<steamrot::InputEvent>(builder.GetBufferPointer());

  auto &event_handler = fixture.GetGameResources().event_handler;

  // Get initial event bus state
  const auto &initial_bus = event_handler.GetGlobalEventBus();
  size_t initial_size = initial_bus.size();

  // Execute the event
  auto result = steamrot::tests::execute_input_event(event, fixture);
  REQUIRE(result.has_value());

  // Process waiting room to move event to global bus
  event_handler.ProcessWaitingRoomEventBus();

  // Verify EventPacket was created and added
  const auto &updated_bus = event_handler.GetGlobalEventBus();
  REQUIRE(updated_bus.size() == initial_size + 1);

  // Verify the event has correct type
  const auto &last_event = updated_bus.back();
  REQUIRE(last_event.m_event_type == steamrot::EventType_EVENT_USER_INPUT);
  REQUIRE(last_event.event_lifetime == 1);

  // Verify mouse position was also updated
  auto &game_context = fixture.GetGameContext();
  REQUIRE(game_context.mouse_position.x == 100);
  REQUIRE(game_context.mouse_position.y == 150);
}

TEST_CASE("execute_input_event generates EventPacket for KeyPress",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create key press event (A key)
  flatbuffers::FlatBufferBuilder builder;
  auto keyboard_data = steamrot::CreateKeyboardInputData(
      builder, static_cast<uint32_t>(sf::Keyboard::Key::A), false, false,
      false);
  auto input_event = steamrot::CreateInputEvent(
      builder, steamrot::InputType_KeyPress,
      steamrot::InputEventData_KeyboardInputData, keyboard_data.Union(), 0);
  builder.Finish(input_event);

  const steamrot::InputEvent *event =
      flatbuffers::GetRoot<steamrot::InputEvent>(builder.GetBufferPointer());

  auto &event_handler = fixture.GetGameResources().event_handler;

  // Get initial event bus state
  const auto &initial_bus = event_handler.GetGlobalEventBus();
  size_t initial_size = initial_bus.size();

  // Execute the event
  auto result = steamrot::tests::execute_input_event(event, fixture);
  REQUIRE(result.has_value());

  // Process waiting room to move event to global bus
  event_handler.ProcessWaitingRoomEventBus();

  // Verify EventPacket was created and added
  const auto &updated_bus = event_handler.GetGlobalEventBus();
  REQUIRE(updated_bus.size() == initial_size + 1);

  // Verify the event has correct type
  const auto &last_event = updated_bus.back();
  REQUIRE(last_event.m_event_type == steamrot::EventType_EVENT_USER_INPUT);
  REQUIRE(last_event.event_lifetime == 1);
}

TEST_CASE("execute_input_event does not generate EventPacket for MouseMove",
          "[unit][harness][input_simulation]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();

  // Create mouse move event
  flatbuffers::FlatBufferBuilder builder;
  auto position = steamrot::CreateVector2fData(builder, 250.0f, 300.0f);
  auto mouse_data = steamrot::CreateMouseInputData(builder, position, 0);
  auto input_event = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse_data.Union(), 0);
  builder.Finish(input_event);

  const steamrot::InputEvent *event =
      flatbuffers::GetRoot<steamrot::InputEvent>(builder.GetBufferPointer());

  auto &event_handler = fixture.GetGameResources().event_handler;

  // Get initial event bus state
  const auto &initial_bus = event_handler.GetGlobalEventBus();
  size_t initial_size = initial_bus.size();

  // Execute the event
  auto result = steamrot::tests::execute_input_event(event, fixture);
  REQUIRE(result.has_value());

  // Process waiting room (should have nothing)
  event_handler.ProcessWaitingRoomEventBus();

  // Verify NO EventPacket was added (MouseMove only updates position)
  const auto &updated_bus = event_handler.GetGlobalEventBus();
  REQUIRE(updated_bus.size() == initial_size);

  // Verify mouse position was still updated
  auto &game_context = fixture.GetGameContext();
  REQUIRE(game_context.mouse_position.x == 250);
  REQUIRE(game_context.mouse_position.y == 300);
}
