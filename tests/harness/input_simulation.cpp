/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to simulate input events for testing
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "input_simulation.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_events_for_tick(const std::vector<InputEvent> &input_events,
                              EventHandler &event_handler,
                              sf::Vector2i &mouse_position) {
  std::vector<sf::Event> sfml_events;

  for (const auto &input_event : input_events) {

    switch (input_event.input_type) {

    case InputType::MouseMove: {
      const auto &data = std::get<MouseInputData>(input_event.input_data);
      mouse_position = {static_cast<int>(data.x), static_cast<int>(data.y)};
      sf::Event::MouseMoved moved;
      moved.position = mouse_position;
      sfml_events.push_back(sf::Event{moved});
      break;
    }

    case InputType::MouseClick: {
      const auto &data = std::get<MouseInputData>(input_event.input_data);
      mouse_position = {static_cast<int>(data.x), static_cast<int>(data.y)};
      sf::Event::MouseButtonPressed pressed;
      pressed.button = static_cast<sf::Mouse::Button>(data.button);
      pressed.position = mouse_position;
      sfml_events.push_back(sf::Event{pressed});
      break;
    }

    case InputType::MouseRelease: {
      const auto &data = std::get<MouseInputData>(input_event.input_data);
      mouse_position = {static_cast<int>(data.x), static_cast<int>(data.y)};
      sf::Event::MouseButtonReleased released;
      released.button = static_cast<sf::Mouse::Button>(data.button);
      released.position = mouse_position;
      sfml_events.push_back(sf::Event{released});
      break;
    }

    case InputType::KeyPress: {
      const auto &data = std::get<KeyInputData>(input_event.input_data);
      sf::Event::KeyPressed pressed;
      pressed.code = static_cast<sf::Keyboard::Key>(data.key_code);
      sfml_events.push_back(sf::Event{pressed});
      break;
    }

    case InputType::KeyRelease: {
      const auto &data = std::get<KeyInputData>(input_event.input_data);
      sf::Event::KeyReleased released;
      released.code = static_cast<sf::Keyboard::Key>(data.key_code);
      sfml_events.push_back(sf::Event{released});
      break;
    }

    default:
      return std::unexpected(
          FailInfo{FailMode::NonExistentEnumValue,
                   "execute_input_events_for_tick: unhandled InputType value"});
    }
  }

  // Feed through the existing SFML event conversion pipeline so that
  // bitset accumulation, input-action resolution and EventPacket creation
  // all happen exactly as they would with real hardware events.
  event_handler.ConvertSFMLEventsToEventPackets(sfml_events);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_sequence(
    const std::unordered_map<size_t, std::vector<InputEvent>>
        &input_events_by_tick,
    size_t current_tick, EventHandler &event_handler,
    sf::Vector2i &mouse_position) {

  const auto it = input_events_by_tick.find(current_tick);
  if (it == input_events_by_tick.end()) {
    return std::monostate{};
  }

  return execute_input_events_for_tick(it->second, event_handler,
                                       mouse_position);
}

} // namespace steamrot::tests
