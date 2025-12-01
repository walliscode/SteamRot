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
#include <format>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InjectInput(const InputEvent *input_event, EventHandler &event_handler,
            GameContext &game_context) {
  {

    // Validate input event
    if (!input_event) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "InputEvent is null"));
    }

    // Create UserInputBitset for the event (if applicable)
    UserInputBitset user_input_bitset;
    bool should_create_event = false;

    // Handle different input types by directly setting the bitset
    switch (input_event->input_type()) {
    case InputType_MouseMove: {
      if (input_event->input_data_type() != InputEventData_MouseInputData) {
        return std::unexpected(
            FailInfo(FailMode::NonExistentEnumValue,
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
        return std::unexpected(
            FailInfo(FailMode::NonExistentEnumValue,
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

      // Set mouse button pressed in the bitset
      user_input_bitset.setMousePressed(
          static_cast<sf::Mouse::Button>(mouse_data->button()));
      should_create_event = true;
      break;
    }

    case InputType_MouseRelease: {
      if (input_event->input_data_type() != InputEventData_MouseInputData) {
        return std::unexpected(
            FailInfo(FailMode::NonExistentEnumValue,
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

      // Set mouse button released in the bitset
      user_input_bitset.setMouseReleased(
          static_cast<sf::Mouse::Button>(mouse_data->button()));
      should_create_event = true;
      break;
    }

    case InputType_KeyPress: {
      if (input_event->input_data_type() != InputEventData_KeyboardInputData) {
        return std::unexpected(
            FailInfo(FailMode::NonExistentEnumValue,
                     "KeyPress input requires KeyboardInputData"));
      }

      const KeyboardInputData *keyboard_data =
          input_event->input_data_as_KeyboardInputData();
      if (!keyboard_data) {
        return std::unexpected(
            FailInfo(FailMode::NullPointer, "KeyboardInputData is null"));
      }

      // Set key pressed in the bitset
      user_input_bitset.setKeyPressed(
          static_cast<sf::Keyboard::Key>(keyboard_data->key_code()));
      should_create_event = true;
      break;
    }

    case InputType_KeyRelease: {
      if (input_event->input_data_type() != InputEventData_KeyboardInputData) {
        return std::unexpected(
            FailInfo(FailMode::NonExistentEnumValue,
                     "KeyRelease input requires KeyboardInputData"));
      }

      const KeyboardInputData *keyboard_data =
          input_event->input_data_as_KeyboardInputData();
      if (!keyboard_data) {
        return std::unexpected(
            FailInfo(FailMode::NullPointer, "KeyboardInputData is null"));
      }

      // Set key released in the bitset
      user_input_bitset.setKeyReleased(
          static_cast<sf::Keyboard::Key>(keyboard_data->key_code()));
      should_create_event = true;
      break;
    }

    default:
      return std::unexpected(
          FailInfo(FailMode::NonExistentEnumValue,
                   std::format("Unknown InputType: {}",
                               static_cast<int>(input_event->input_type()))));
    }

    // If we should create an event, create EventPacket and add to EventHandler
    if (should_create_event) {
      // Create EventPacket with the UserInputBitset
      EventPacket event_packet(EventType_EVENT_USER_INPUT, user_input_bitset,
                               1);

      // Add to EventHandler
      event_handler.AddEvent(event_packet);
    }
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> InjectInputsForTick(
    const flatbuffers::Vector<flatbuffers::Offset<InputEvent>> *inputs,
    EventHandler &event_handler, GameContext &game_context) {

  // Validate input sequence
  if (!inputs) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "InputSequence is null"));
  }

  // Process all events for this tick
  for (const InputEvent *input_event : *inputs) {
    if (!input_event) {
      continue; // Skip null events
    }

    auto result = InjectInput(input_event, event_handler, game_context);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }

  return std::monostate{};
}
} // namespace steamrot::tests
