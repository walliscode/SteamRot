////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "EventHandler.h"
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <optional>
namespace steamrot {
////////////////////////////////////////////////////////////
void EventHandler::HandleEvents(sf::RenderWindow &window) {

  // reset the event bitsets
  m_events.reset();

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
    // handle keyboard events
    else if (event->is<sf::Event::KeyPressed>() ||
             event->is<sf::Event::KeyReleased>()) {
      HandleKeyboardEvents(event.value());
    }
    // handle mouse events
    else if (event->is<sf::Event::MouseButtonPressed>() ||
             event->is<sf::Event::MouseButtonReleased>()) {
      HandleMouseEvents(event.value());
    }
  }
}

////////////////////////////////////////////////////////////
void EventHandler::HandleKeyboardEvents(const sf::Event &event) {

  // if key pressed, set the corresponding bit in the pressed events bitset
  if (const auto *keyPressed = event.getIf<sf::Event::KeyPressed>()) {

    std::cout << "Key pressed: " << magic_enum::enum_name(keyPressed->scancode)
              << std::endl;

    // modify relevant bitset
    m_events.set(static_cast<size_t>(keyPressed->scancode));

  } else if (const auto *keyReleased = event.getIf<sf::Event::KeyReleased>()) {

    std::cout << "Key released: "
              << magic_enum::enum_name(keyReleased->scancode) << std::endl;

    // if key released, then remember add the key count
    m_events.set(static_cast<size_t>(keyReleased->scancode) +
                 sf::Keyboard::KeyCount);
  }
}
////////////////////////////////////////////////////////////
void EventHandler::HandleMouseEvents(const sf::Event &event) {

  // if mouse button pressed, set the corresponding bit in the pressed events
  if (const auto *mouseButtonPressed =
          event.getIf<sf::Event::MouseButtonPressed>()) {

    std::cout << "Mouse button pressed: "
              << magic_enum::enum_name(mouseButtonPressed->button) << std::endl;
    // shift the mouse button by the number of keys x 2
    m_events.set(static_cast<size_t>(mouseButtonPressed->button) +
                 static_cast<size_t>(sf::Keyboard::KeyCount * 2));

  } else if (const auto *mouseButtonReleased =
                 event.getIf<sf::Event::MouseButtonReleased>()) {

    std::cout << "Mouse button released: "
              << magic_enum::enum_name(mouseButtonReleased->button)
              << std::endl;
    // if mouse button released, then remember to add the key count x 2 and the
    // mouse button count
    m_events.set(static_cast<size_t>(mouseButtonReleased->button) +
                 static_cast<size_t>((sf::Keyboard::KeyCount * 2) +
                                     sf::Mouse::ButtonCount));
  }
}

/////////////////////////////////////////////////
const EventBitset &EventHandler::GetEvents() const { return m_events; }

} // namespace steamrot
