/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for input simulation functionality
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "input_simulation.h"
#include "EventHandler.h"
#include "GameContext.h"
#include "input_test_data_generated.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("InjectInput handles null input",
          "[unit][harness][input_simulation]") {
  steamrot::EventHandler event_handler;
  steamrot::GameContext game_context;

  auto result =
      steamrot::tests::InjectInput(nullptr, event_handler, game_context);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("InjectInputsForTick handles null inputs vector",
          "[unit][harness][input_simulation]") {
  steamrot::EventHandler event_handler;
  steamrot::GameContext game_context;

  auto result =
      steamrot::tests::InjectInputsForTick(nullptr, event_handler, game_context);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("InjectInput updates mouse position for MouseMove",
          "[unit][harness][input_simulation]") {
  steamrot::EventHandler event_handler;
  steamrot::GameContext game_context;

  // Create mouse move event
  flatbuffers::FlatBufferBuilder builder;
  auto position = steamrot::CreateVector2fData(builder, 150.0f, 200.0f);
  auto mouse_data = steamrot::CreateMouseInputData(builder, position, 0);
  auto input_event = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse_data.Union(), 1);
  builder.Finish(input_event);

  const steamrot::InputEvent *event =
      flatbuffers::GetRoot<steamrot::InputEvent>(builder.GetBufferPointer());

  // Execute the event
  auto result =
      steamrot::tests::InjectInput(event, event_handler, game_context);
  REQUIRE(result.has_value());

  // Verify mouse position was updated
  REQUIRE(game_context.mouse_position.x == 150);
  REQUIRE(game_context.mouse_position.y == 200);
}

TEST_CASE("InjectInput generates EventPacket for MouseClick",
          "[unit][harness][input_simulation]") {
  steamrot::EventHandler event_handler;
  steamrot::GameContext game_context;

  // Create mouse click event
  flatbuffers::FlatBufferBuilder builder;
  auto position = steamrot::CreateVector2fData(builder, 100.0f, 150.0f);
  auto mouse_data = steamrot::CreateMouseInputData(
      builder, position, static_cast<uint8_t>(sf::Mouse::Button::Left));
  auto input_event = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseClick,
      steamrot::InputEventData_MouseInputData, mouse_data.Union(), 1);
  builder.Finish(input_event);

  const steamrot::InputEvent *event =
      flatbuffers::GetRoot<steamrot::InputEvent>(builder.GetBufferPointer());

  // Get initial event bus state
  const auto &initial_bus = event_handler.GetGlobalEventBus();
  size_t initial_size = initial_bus.size();

  // Execute the event
  auto result =
      steamrot::tests::InjectInput(event, event_handler, game_context);
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
  REQUIRE(game_context.mouse_position.x == 100);
  REQUIRE(game_context.mouse_position.y == 150);
}

TEST_CASE("InjectInput generates EventPacket for KeyPress",
          "[unit][harness][input_simulation]") {
  steamrot::EventHandler event_handler;
  steamrot::GameContext game_context;

  // Create key press event (A key)
  flatbuffers::FlatBufferBuilder builder;
  auto keyboard_data = steamrot::CreateKeyboardInputData(
      builder, static_cast<uint32_t>(sf::Keyboard::Key::A), false, false,
      false);
  auto input_event = steamrot::CreateInputEvent(
      builder, steamrot::InputType_KeyPress,
      steamrot::InputEventData_KeyboardInputData, keyboard_data.Union(), 1);
  builder.Finish(input_event);

  const steamrot::InputEvent *event =
      flatbuffers::GetRoot<steamrot::InputEvent>(builder.GetBufferPointer());

  // Get initial event bus state
  const auto &initial_bus = event_handler.GetGlobalEventBus();
  size_t initial_size = initial_bus.size();

  // Execute the event
  auto result =
      steamrot::tests::InjectInput(event, event_handler, game_context);
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

TEST_CASE("InjectInput does not generate EventPacket for MouseMove",
          "[unit][harness][input_simulation]") {
  steamrot::EventHandler event_handler;
  steamrot::GameContext game_context;

  // Create mouse move event
  flatbuffers::FlatBufferBuilder builder;
  auto position = steamrot::CreateVector2fData(builder, 250.0f, 300.0f);
  auto mouse_data = steamrot::CreateMouseInputData(builder, position, 0);
  auto input_event = steamrot::CreateInputEvent(
      builder, steamrot::InputType_MouseMove,
      steamrot::InputEventData_MouseInputData, mouse_data.Union(), 1);
  builder.Finish(input_event);

  const steamrot::InputEvent *event =
      flatbuffers::GetRoot<steamrot::InputEvent>(builder.GetBufferPointer());

  // Get initial event bus state
  const auto &initial_bus = event_handler.GetGlobalEventBus();
  size_t initial_size = initial_bus.size();

  // Execute the event
  auto result =
      steamrot::tests::InjectInput(event, event_handler, game_context);
  REQUIRE(result.has_value());

  // Process waiting room (should have nothing)
  event_handler.ProcessWaitingRoomEventBus();

  // Verify NO EventPacket was added (MouseMove only updates position)
  const auto &updated_bus = event_handler.GetGlobalEventBus();
  REQUIRE(updated_bus.size() == initial_size);

  // Verify mouse position was still updated
  REQUIRE(game_context.mouse_position.x == 250);
  REQUIRE(game_context.mouse_position.y == 300);
}
