/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the UserInputBitset struct
/////////////////////////////////////////////////

#include "UserInputBitset.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <map>
#include <vector>

using namespace steamrot;

TEST_CASE("UserInputBitset default constructor resets all bits",
          "[unit][UserInputBitset]") {
  UserInputBitset input;

  REQUIRE(input.none());
}

TEST_CASE("UserInputBitset sets key pressed and released bits correctly",
          "[unit][UserInputBitset]") {
  UserInputBitset input;
  input.setKeyPressed(sf::Keyboard::Key::A);
  REQUIRE(input.test(static_cast<size_t>(sf::Keyboard::Key::A)));
  input.setKeyReleased(sf::Keyboard::Key::A);
  REQUIRE(input.test(static_cast<size_t>(sf::Keyboard::Key::A) +
                     sf::Keyboard::KeyCount));
}

TEST_CASE("UserInputBitset sets mouse pressed and released bits correctly",
          "[unit][UserInputBitset]") {
  UserInputBitset input;
  input.setMousePressed(sf::Mouse::Button::Left);
  REQUIRE(
      input.test(kKeyboardBits + static_cast<size_t>(sf::Mouse::Button::Left)));
  input.setMouseReleased(sf::Mouse::Button::Left);
  REQUIRE(input.test(kKeyboardBits +
                     static_cast<size_t>(sf::Mouse::Button::Left) +
                     sf::Mouse::ButtonCount));
}

TEST_CASE("UserInputBitset resets all bits", "[unit][UserInputBitset]") {
  UserInputBitset input;
  input.setKeyPressed(sf::Keyboard::Key::A);
  input.setMousePressed(sf::Mouse::Button::Left);
  input.reset();

  REQUIRE(input.none());
}

TEST_CASE(
    "UserInputBitset constructor from vector of events sets bits correctly",
    "[unit][UserInputBitset]") {
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
          "[unit][UserInputBitset]") {
  UserInputBitset a;
  UserInputBitset b;
  REQUIRE(a == b);

  a.setKeyPressed(sf::Keyboard::Key::A);
  REQUIRE_FALSE(a == b);

  b.setKeyPressed(sf::Keyboard::Key::A);
  REQUIRE(a == b);
}

TEST_CASE("UserInputBitset less-than operator provides strict weak ordering",
          "[unit][UserInputBitset]") {
  UserInputBitset empty;

  UserInputBitset with_a;
  with_a.setKeyPressed(sf::Keyboard::Key::A);

  UserInputBitset with_b;
  with_b.setKeyPressed(sf::Keyboard::Key::B);

  // Irreflexivity: a < a must be false.
  REQUIRE_FALSE(empty < empty);
  REQUIRE_FALSE(with_a < with_a);

  // Antisymmetry: if a < b then b < a must be false.
  if (with_a < with_b) {
    REQUIRE_FALSE(with_b < with_a);
  } else {
    REQUIRE(with_b < with_a);
  }

  // Works as a map key (no compile error and correct deduplication).
  std::map<UserInputBitset, int> m;
  m.emplace(with_a, 1);
  m.emplace(with_a, 2); // duplicate key — emplace does not overwrite, original retained
  REQUIRE(m.size() == 1);
  REQUIRE(m.at(with_a) == 1);
}
