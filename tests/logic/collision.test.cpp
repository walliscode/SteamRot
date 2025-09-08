/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for collsion detection functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision.h"
#include "catch2/generators/catch_generators.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("IsMouseOverBounds returns false for point outside bounds",
          "[collision]") {
  sf::Vector2i mouse_position(150, 150);
  sf::FloatRect bounds({0, 0}, {100, 100});

  bool result = steamrot::collision::IsMouseOverBounds(mouse_position, bounds);
  REQUIRE(result == false);
}

// Helper for easy construction
struct TestCase {
  sf::Vector2i mouse_pos;
  sf::FloatRect bounds;
  bool expected;
};

TEST_CASE("IsMouseOverBounds returns correct results for various bounds and "
          "mouse positions",
          "[collision]") {
  auto cases = GENERATE(
      TestCase{{50, 50}, sf::FloatRect({0, 0}, {100, 100}), true},
      TestCase{{0, 0}, sf::FloatRect({0, 0}, {100, 100}), true},
      TestCase{{99, 99}, sf::FloatRect({0, 0}, {100, 100}), true},
      TestCase{{100, 100}, sf::FloatRect({0, 0}, {100, 100}), false},
      TestCase{{-1, 50}, sf::FloatRect({0, 0}, {100, 100}), false},
      TestCase{{50, -1}, sf::FloatRect({0, 0}, {100, 100}), false},
      TestCase{{150, 50}, sf::FloatRect({100, 0}, {50, 50}), false},
      TestCase{{125, 25}, sf::FloatRect({100, 0}, {50, 50}), true},
      TestCase{{149, 49}, sf::FloatRect({100, 0}, {50, 50}), true},
      TestCase{{150, 50}, sf::FloatRect({100, 0}, {50, 50}), false},
      TestCase{{0, 0}, sf::FloatRect({10, 10}, {30, 30}), false},
      TestCase{{15, 15}, sf::FloatRect({10, 10}, {30, 30}), true},
      TestCase{{40, 40}, sf::FloatRect({10, 10}, {30, 30}), false},
      TestCase{{39, 39}, sf::FloatRect({10, 10}, {30, 30}), true},
      TestCase{{200, 200}, sf::FloatRect({190, 190}, {10, 10}), false},
      TestCase{{199, 199}, sf::FloatRect({190, 190}, {10, 10}), true},
      TestCase{{189, 189}, sf::FloatRect({190, 190}, {10, 10}), false},
      TestCase{{300, 400}, sf::FloatRect({250, 350}, {60, 70}), true},
      TestCase{{310, 420}, sf::FloatRect({250, 350}, {60, 70}), false},
      TestCase{{320, 430}, sf::FloatRect({250, 350}, {60, 70}), false});

  CAPTURE(cases.mouse_pos.x, cases.mouse_pos.y,
          static_cast<float>(cases.mouse_pos.x),
          static_cast<float>(cases.mouse_pos.y), cases.bounds.position.x,
          cases.bounds.position.y,
          (cases.bounds.position.x + cases.bounds.size.x), cases.expected);
  bool result =
      steamrot::collision::IsMouseOverBounds(cases.mouse_pos, cases.bounds);
  REQUIRE(result == cases.expected);
}
