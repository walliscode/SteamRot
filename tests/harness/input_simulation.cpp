/////////////////////////////////////////////////
/// @file
/// @brief Implementation of input simulation utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "input_simulation.h"
#include "GameContext.h"
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <format>
#include <set>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_event(const InputEvent *input_event, TestFixture &fixture) {

  // Validate input event
  if (!input_event) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "InputEvent is null"));
  }

  // Get game context for accessing mouse position
  GameContext &game_context = fixture.GetGameContext();

  // Handle different input types
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
    break;
  }

  case InputType_MouseClick:
  case InputType_MouseRelease: {
    if (input_event->input_data_type() != InputEventData_MouseInputData) {
      return std::unexpected(FailInfo(
          FailMode::InvalidEnumValue,
          "MouseClick/MouseRelease input requires MouseInputData"));
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

    // Note: Mouse button state tracking would require extending GameContext
    // or SceneContext to store button states. For now, we just update position.
    // The actual button processing would happen in collision/action logic.
    break;
  }

  case InputType_KeyPress:
  case InputType_KeyRelease: {
    if (input_event->input_data_type() != InputEventData_KeyboardInputData) {
      return std::unexpected(FailInfo(
          FailMode::InvalidEnumValue,
          "KeyPress/KeyRelease input requires KeyboardInputData"));
    }

    const KeyboardInputData *keyboard_data =
        input_event->input_data_as_KeyboardInputData();
    if (!keyboard_data) {
      return std::unexpected(
          FailInfo(FailMode::NullPointer, "KeyboardInputData is null"));
    }

    // Note: Keyboard state tracking would require extending GameContext
    // or SceneContext to store key states. For now, this is a placeholder.
    // The actual key processing would happen in action logic.
    break;
  }

  default:
    return std::unexpected(FailInfo(
        FailMode::InvalidEnumValue,
        std::format("Unknown InputType: {}",
                    static_cast<int>(input_event->input_type()))));
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
