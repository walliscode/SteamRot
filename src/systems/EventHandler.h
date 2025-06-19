////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <bitset>
#include <unordered_map>

// double of each needed for pressed and released events
using EventBitset =
    std::bitset<sf::Keyboard::KeyCount + sf::Keyboard::KeyCount +
                sf::Mouse::ButtonCount + sf::Mouse::ButtonCount>;

using UserEvents = std::unordered_map<std::string, EventBitset>;
namespace steamrot {

class EventHandler {

private:
  ////////////////////////////////////////////////////////////
  // |brief process keyboard events: pressed and released
  ////////////////////////////////////////////////////////////
  void HandleKeyboardEvents(const sf::Event &event, UserEvents &events);

  ////////////////////////////////////////////////////////////
  // |brief process mouse events: pressed and released
  ////////////////////////////////////////////////////////////
  void HandleMouseEvents(const sf::Event &event, UserEvents &events);

  EventBitset m_pressed_events;
  EventBitset m_released_events;

public:
  ////////////////////////////////////////////////////////////
  // |brief default constructor
  ////////////////////////////////////////////////////////////
  EventHandler() = default;

  ////////////////////////////////////////////////////////////
  // |brief process events from the window
  ////////////////////////////////////////////////////////////
  UserEvents HandleEvents(sf::RenderWindow &window);

  ////////////////////////////////////////////////////////////
  // |member: bool queried by GameEngine about whether to close the window or
  // not
  ////////////////////////////////////////////////////////////
  bool m_close_window{false};

  const EventBitset &GetPressedEvents() const;

  const EventBitset &GetReleasedEvents() const;
};
} // namespace steamrot
