/////////////////////////////////////////////////
/// @file
/// @brief Unit test for the Style struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Style.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Style: Default Constructor") {
  Style style;

  REQUIRE(style.background_color.r == 0);
  REQUIRE(style.background_color.g == 0);
  REQUIRE(style.background_color.b == 0);
  REQUIRE(style.background_color.a == 255);

  REQUIRE(style.border_color.r == 0);
  REQUIRE(style.border_color.g == 0);
  REQUIRE(style.border_color.b == 0);
  REQUIRE(style.border_color.a == 255);

  REQUIRE(style.border_thickness == 0.0f);
  REQUIRE(style.radius_resolution == 0);
  REQUIRE(style.inner_margin == sf::Vector2f(0.0f, 0.0f));
  REQUIRE(style.minimum_size == sf::Vector2f(0.0f, 0.0f));
  REQUIRE(style.maximum_size == sf::Vector2f(0.0f, 0.0f));
}
