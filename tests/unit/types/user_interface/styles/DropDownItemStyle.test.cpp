/////////////////////////////////////////////////
/// @file
/// @brief Unit test for the DropDownItemStyle struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DropDownItemStyle.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("DropDownItemStyle - Default Constructor", "[DropDownItemStyle]") {
  DropDownItemStyle style;

  // check underlying style defaults
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

  // check DropDownItemStyle specific defaults
  REQUIRE(style.text_color.r == 0);
  REQUIRE(style.text_color.g == 0);
  REQUIRE(style.text_color.b == 0);
  REQUIRE(style.text_color.a == 255);
  REQUIRE(style.hover_color.r == 0);
  REQUIRE(style.hover_color.g == 0);
  REQUIRE(style.hover_color.b == 0);
  REQUIRE(style.hover_color.a == 255);
  REQUIRE(style.font == nullptr);
  REQUIRE(style.font_size == 0);
}
