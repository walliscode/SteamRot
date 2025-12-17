/////////////////////////////////////////////////
/// @file
/// @brief Unit test for the UIStyle struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIStyle.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIStyle - Default Constructor", "[UIStyle]") {
  steamrot::UIStyle style;

  // check name defaults
  REQUIRE(style.name == "style_name");

  // check panel_style defaults (inherits from Style)
  REQUIRE(style.panel_style.background_color.r == 0);
  REQUIRE(style.panel_style.background_color.g == 0);
  REQUIRE(style.panel_style.background_color.b == 0);
  REQUIRE(style.panel_style.background_color.a == 255);
  REQUIRE(style.panel_style.border_thickness == 0.0f);

  // check button_style defaults (inherits from Style)
  REQUIRE(style.button_style.background_color.r == 0);
  REQUIRE(style.button_style.text_color.r == 0);
  REQUIRE(style.button_style.hover_color.r == 0);
  REQUIRE(style.button_style.font == nullptr);
  REQUIRE(style.button_style.font_size == 0);

  // check drop_down_container_style defaults (inherits from Style)
  REQUIRE(style.drop_down_container_style.background_color.r == 0);
  REQUIRE(style.drop_down_container_style.drop_symbol_ratio == 0.2f);

  // check drop_down_item_style defaults (inherits from Style)
  REQUIRE(style.drop_down_item_style.background_color.r == 0);
  REQUIRE(style.drop_down_item_style.text_color.r == 0);
  REQUIRE(style.drop_down_item_style.hover_color.r == 0);
  REQUIRE(style.drop_down_item_style.font == nullptr);
  REQUIRE(style.drop_down_item_style.font_size == 0);

  // check drop_down_list_style defaults (inherits from Style)
  REQUIRE(style.drop_down_list_style.background_color.r == 0);
  REQUIRE(style.drop_down_list_style.text_color.r == 0);
  REQUIRE(style.drop_down_list_style.hover_color.r == 0);
  REQUIRE(style.drop_down_list_style.font == nullptr);
  REQUIRE(style.drop_down_list_style.font_size == 12);

  // check drop_down_button_style defaults (inherits from Style)
  REQUIRE(style.drop_down_button_style.background_color.r == 0);
  REQUIRE(style.drop_down_button_style.triangle_color.r == 0);
  REQUIRE(style.drop_down_button_style.hover_color.r == 0);
}
