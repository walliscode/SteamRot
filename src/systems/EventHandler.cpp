////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "EventHandler.h"
#include <SFML/Window/Event.hpp>
#include <optional>

namespace steamrot {
////////////////////////////////////////////////////////////
UserEvents EventHandler::HandleEvents(sf::RenderWindow &window) {

  UserEvents event_bitsets;

  // create a bitset for pressed events and add to the map
  std::bitset<sf::Keyboard::KeyCount + sf::Mouse::ButtonCount> pressed_events;
  event_bitsets["pressed"] = pressed_events;
  // create a bitset for released events and add to the map
  std::bitset<sf::Keyboard::KeyCount + sf::Mouse::ButtonCount> released_events;
  event_bitsets["released"] = released_events;

  // poll events from the window
  while (const std::optional<sf::Event> event = window.pollEvent()) {
    // add in ctrl + c to close the window
    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
        window.close();
    } // if the event is a close event, set the close window flag to true
    if (event->is<sf::Event::Closed>()) {
      window.close();
    }
    // // handle keyboard events
    // else if (event->is<sf::Event::KeyPressed>() ||
    //          event->is<sf::Event::KeyReleased>()) {
    //   HandleKeyboardEvents(event.value(), event_bitsets);
    // }
    // // handle mouse events
    // else if (event->is<sf::Event::MouseButtonPressed>() ||
    //          event->is<sf::Event::MouseButtonReleased>()) {
    //   HandleMouseEvents(event.value(), event_bitsets);
    // }
  }
  return event_bitsets;
}

////////////////////////////////////////////////////////////
void EventHandler::HandleKeyboardEvents(const sf::Event &event,
                                        UserEvents &event_bitsets) {

  // if key pressed, set the corresponding bit in the pressed events bitset
  if (const auto *keyPressed = event.getIf<sf::Event::KeyPressed>()) {

    auto pressed_bitset = event_bitsets["pressed"];

    // modify relevant bitset
    pressed_bitset.set(static_cast<size_t>(keyPressed->scancode));

  } else if (const auto *keyReleased = event.getIf<sf::Event::KeyReleased>()) {

    // if key released, set the corresponding bit in the released events bitset
    auto released_bitset = event_bitsets["released"];
    // modify relevant bitset
    released_bitset.set(static_cast<size_t>(keyReleased->scancode));
  }
}
////////////////////////////////////////////////////////////
void EventHandler::HandleMouseEvents(const sf::Event &event,
                                     UserEvents &event_bitsets) {

  // if mouse button pressed, set the corresponding bit in the pressed events
  if (const auto *mouseButtonPressed =
          event.getIf<sf::Event::MouseButtonPressed>()) {

    auto pressed_bitset = event_bitsets["pressed"];
    // modify relevant bitset
    pressed_bitset.set(static_cast<size_t>(mouseButtonPressed->button) +
                       static_cast<size_t>(sf::Keyboard::KeyCount));

  } else if (const auto *mouseButtonReleased =
                 event.getIf<sf::Event::MouseButtonReleased>()) {

    // if mouse button released, set the corresponding bit in the released
    // events bitset
    auto released_bitset = event_bitsets["released"];
    // modify relevant bitset
    released_bitset.set(static_cast<size_t>(mouseButtonReleased->button) +
                        static_cast<size_t>(sf::Keyboard::KeyCount));
  }
}
} // namespace steamrot
