/////////////////////////////////////////////////
/// @file
/// @brief Implementation of input simulation utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "input_simulation.h"
#include "EventHandler.h"
#include "EventPacket.h"
#include "GameContext.h"
#include "UserInputBitset.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <format>
#include <set>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_event(const InputEvent *input_event, TestFixture &fixture) {

  // Validate input event
  if (!input_event) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "InputEvent is null"));
  }

  // Get game context for accessing mouse position and event handler
  GameContext &game_context = fixture.GetGameContext();
  EventHandler &event_handler = fixture.GetGameResources().event_handler;

  // Create a vector of SFML events to convert to UserInputBitset
  std::vector<sf::Event> sfml_events;

  // Handle different input types by creating SFML events
  switch (input_event->input_type()) {
  case InputType_MouseMove: {
    if (input_event->input_data_type() != InputEventData_MouseInputData) {
      return std::unexpected(
          FailInfo(FailMode::InvalidEnumValue,
                   "MouseMove input requires MouseInputData"));
    }

    const MouseInputData *mouse_data =
        input_event->input_data_as_MouseInputData();
    if (!mouse_data) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "MouseInputData is null"));
    }

    if (!mouse_data->position()) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "MouseInputData position is null"));
    }

    // Update mouse position in game context
    game_context.mouse_position.x =
        static_cast<int>(mouse_data->position()->x());
    game_context.mouse_position.y =
        static_cast<int>(mouse_data->position()->y());

    // MouseMove doesn't generate a UserInput event (no button/key change)
    return std::monostate{};
  }

  case InputType_MouseClick: {
    if (input_event->input_data_type() != InputEventData_MouseInputData) {
      return std::unexpected(FailInfo(
          FailMode::InvalidEnumValue,
          "MouseClick input requires MouseInputData"));
    }

    const MouseInputData *mouse_data =
        input_event->input_data_as_MouseInputData();
    if (!mouse_data) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "MouseInputData is null"));
    }

    if (!mouse_data->position()) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "MouseInputData position is null"));
    }

    // Update mouse position
    game_context.mouse_position.x =
        static_cast<int>(mouse_data->position()->x());
    game_context.mouse_position.y =
        static_cast<int>(mouse_data->position()->y());

    // Create SFML MouseButtonPressed event
    sf::Event::MouseButtonPressed mouse_pressed;
    mouse_pressed.button = static_cast<sf::Mouse::Button>(mouse_data->button());
    mouse_pressed.position.x = static_cast<int>(mouse_data->position()->x());
    mouse_pressed.position.y = static_cast<int>(mouse_data->position()->y());
    sfml_events.push_back(sf::Event{mouse_pressed});
    break;
  }

  case InputType_MouseRelease: {
    if (input_event->input_data_type() != InputEventData_MouseInputData) {
      return std::unexpected(FailInfo(
          FailMode::InvalidEnumValue,
          "MouseRelease input requires MouseInputData"));
    }

    const MouseInputData *mouse_data =
        input_event->input_data_as_MouseInputData();
    if (!mouse_data) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "MouseInputData is null"));
    }

    if (!mouse_data->position()) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "MouseInputData position is null"));
    }

    // Update mouse position
    game_context.mouse_position.x =
        static_cast<int>(mouse_data->position()->x());
    game_context.mouse_position.y =
        static_cast<int>(mouse_data->position()->y());

    // Create SFML MouseButtonReleased event
    sf::Event::MouseButtonReleased mouse_released;
    mouse_released.button = static_cast<sf::Mouse::Button>(mouse_data->button());
    mouse_released.position.x = static_cast<int>(mouse_data->position()->x());
    mouse_released.position.y = static_cast<int>(mouse_data->position()->y());
    sfml_events.push_back(sf::Event{mouse_released});
    break;
  }

  case InputType_KeyPress: {
    if (input_event->input_data_type() != InputEventData_KeyboardInputData) {
      return std::unexpected(FailInfo(
          FailMode::InvalidEnumValue,
          "KeyPress input requires KeyboardInputData"));
    }

    const KeyboardInputData *keyboard_data =
        input_event->input_data_as_KeyboardInputData();
    if (!keyboard_data) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "KeyboardInputData is null"));
    }

    // Create SFML KeyPressed event
    sf::Event::KeyPressed key_pressed;
    key_pressed.code = static_cast<sf::Keyboard::Key>(keyboard_data->key_code());
    key_pressed.alt = keyboard_data->alt();
    key_pressed.control = keyboard_data->control();
    key_pressed.shift = keyboard_data->shift();
    key_pressed.system = false;
    sfml_events.push_back(sf::Event{key_pressed});
    break;
  }

  case InputType_KeyRelease: {
    if (input_event->input_data_type() != InputEventData_KeyboardInputData) {
      return std::unexpected(FailInfo(
          FailMode::InvalidEnumValue,
          "KeyRelease input requires KeyboardInputData"));
    }

    const KeyboardInputData *keyboard_data =
        input_event->input_data_as_KeyboardInputData();
    if (!keyboard_data) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "KeyboardInputData is null"));
    }

    // Create SFML KeyReleased event
    sf::Event::KeyReleased key_released;
    key_released.code = static_cast<sf::Keyboard::Key>(keyboard_data->key_code());
    key_released.alt = keyboard_data->alt();
    key_released.control = keyboard_data->control();
    key_released.shift = keyboard_data->shift();
    key_released.system = false;
    sfml_events.push_back(sf::Event{key_released});
    break;
  }

  default:
    return std::unexpected(FailInfo(
        FailMode::InvalidEnumValue,
        std::format("Unknown InputType: {}",
                    static_cast<int>(input_event->input_type()))));
  }

  // If we have SFML events, create an EventPacket and add to EventHandler
  if (!sfml_events.empty()) {
    // Create UserInputBitset from SFML events
    UserInputBitset user_input_bitset(sfml_events);

    // Create EventPacket
    EventPacket event_packet(EventType_EVENT_USER_INPUT, user_input_bitset, 1);

    // Add to EventHandler
    event_handler.AddEvent(event_packet);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_events_for_tick(const InputSequence *input_sequence,
                               uint32_t tick, TestFixture &fixture) {

  // Validate input sequence
  if (!input_sequence) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "InputSequence is null"));
  }

  if (!input_sequence->inputs()) {
    // No inputs to process - not an error
    return std::monostate{};
  }

  // Process all events for this tick
  for (const InputEvent *input_event : *input_sequence->inputs()) {
    if (!input_event) {
      continue; // Skip null events
    }

    if (input_event->tick() == tick) {
      auto result = execute_input_event(input_event, fixture);
      if (!result.has_value()) {
        return std::unexpected(result.error());
      }
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_sequence(const InputSequence *input_sequence,
                       TestFixture &fixture) {

  // Validate input sequence
  if (!input_sequence) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "InputSequence is null"));
  }

  if (!input_sequence->inputs() || input_sequence->inputs()->size() == 0) {
    // No inputs to process - not an error
    return std::monostate{};
  }

  // Collect all unique tick values
  std::set<uint32_t> ticks;
  for (const InputEvent *input_event : *input_sequence->inputs()) {
    if (input_event) {
      ticks.insert(input_event->tick());
    }
  }

  // Process events for each tick in order
  for (uint32_t tick : ticks) {
    auto result = execute_input_events_for_tick(input_sequence, tick, fixture);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }

  return std::monostate{};
}

} // namespace steamrot::tests
