/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for ColorEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ColorEqualsMatcher.h"
#include <SFML/Graphics/Color.hpp>
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("ColorEqualsMatcher works correctly", "[unit][Color][matcher]") {
  sf::Color expected{255, 0, 0, 255}; // Red
  sf::Color actual;

  SECTION("Matcher detects differences in red component") {
    actual = sf::Color{0, 0, 0, 255}; // Black
    REQUIRE_THAT(actual, !steamrot::tests::EqualsColor(expected));
  }

  SECTION("Matcher detects differences in green component") {
    actual = sf::Color{255, 255, 0, 255}; // Yellow
    REQUIRE_THAT(actual, !steamrot::tests::EqualsColor(expected));
  }

  SECTION("Matcher detects differences in blue component") {
    actual = sf::Color{255, 0, 255, 255}; // Magenta
    REQUIRE_THAT(actual, !steamrot::tests::EqualsColor(expected));
  }

  SECTION("Matcher detects differences in alpha component") {
    actual = sf::Color{255, 0, 0, 128}; // Semi-transparent red
    REQUIRE_THAT(actual, !steamrot::tests::EqualsColor(expected));
  }

  SECTION("Matcher detects equality") {
    actual = sf::Color{255, 0, 0, 255}; // Red
    REQUIRE_THAT(actual, steamrot::tests::EqualsColor(expected));
  }
}

} // namespace steamrot::tests
