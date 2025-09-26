/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the UserInputBitset struct
/////////////////////////////////////////////////

#include "UserInputBitset.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

using namespace steamrot;

TEST_CASE("UserInputBitset default constructor resets all bits",
          "[UserInputBitset]") {
  UserInputBitset input;
  for (size_t i = 0; i < kTotalBits; ++i) {
    REQUIRE_FALSE(input.test(i));
  }
}

TEST_CASE("UserInputBitset sets key pressed and released bits correctly",
          "[UserInputBitset]") {
  UserInputBitset input;
  input.setKeyPressed(sf::Keyboard::Key::A);
  REQUIRE(input.test(static_cast<size_t>(sf::Keyboard::Key::A)));
  input.setKeyReleased(sf::Keyboard::Key::A);
  REQUIRE(input.test(static_cast<size_t>(sf::Keyboard::Key::A) +
                     sf::Keyboard::KeyCount));
}

TEST_CASE("UserInputBitset sets mouse pressed and released bits correctly",
          "[UserInputBitset]") {
  UserInputBitset input;
  input.setMousePressed(sf::Mouse::Button::Left);
  REQUIRE(
      input.test(kKeyboardBits + static_cast<size_t>(sf::Mouse::Button::Left)));
  input.setMouseReleased(sf::Mouse::Button::Left);
  REQUIRE(input.test(kKeyboardBits +
                     static_cast<size_t>(sf::Mouse::Button::Left) +
                     sf::Mouse::ButtonCount));
}

TEST_CASE("UserInputBitset resets all bits", "[UserInputBitset]") {
  UserInputBitset input;
  input.setKeyPressed(sf::Keyboard::Key::A);
  input.setMousePressed(sf::Mouse::Button::Left);
  input.reset();
  for (size_t i = 0; i < kTotalBits; ++i) {
    REQUIRE_FALSE(input.test(i));
  }
}

TEST_CASE(
    "UserInputBitset constructor from vector of events sets bits correctly",
    "[UserInputBitset]") {
  std::vector<sf::Event> events;

  // Use event variants directly
  sf::Event::KeyPressed keyPressEvent;
  keyPressEvent.code = sf::Keyboard::Key::A;
  events.emplace_back(keyPressEvent);

  sf::Event::KeyReleased keyReleaseEvent;
  keyReleaseEvent.code = sf::Keyboard::Key::B;
  events.emplace_back(keyReleaseEvent);

  sf::Event::MouseButtonPressed mousePressEvent;
  mousePressEvent.button = sf::Mouse::Button::Left;
  events.emplace_back(mousePressEvent);

  sf::Event::MouseButtonReleased mouseReleaseEvent;
  mouseReleaseEvent.button = sf::Mouse::Button::Right;
  events.emplace_back(mouseReleaseEvent);

  UserInputBitset input(events);

  REQUIRE(input.test(static_cast<size_t>(sf::Keyboard::Key::A)));
  REQUIRE(input.test(static_cast<size_t>(sf::Keyboard::Key::B) +
                     sf::Keyboard::KeyCount));
  REQUIRE(
      input.test(kKeyboardBits + static_cast<size_t>(sf::Mouse::Button::Left)));
  REQUIRE(input.test(kKeyboardBits +
                     static_cast<size_t>(sf::Mouse::Button::Right) +
                     sf::Mouse::ButtonCount));
}

TEST_CASE("UserInputBitset equality operator works as expected",
          "[UserInputBitset]") {
  UserInputBitset a;
  UserInputBitset b;
  REQUIRE(a == b);

  a.setKeyPressed(sf::Keyboard::Key::A);
  REQUIRE_FALSE(a == b);

  b.setKeyPressed(sf::Keyboard::Key::A);
  REQUIRE(a == b);
}
