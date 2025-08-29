/////////////////////////////////////////////////
/// @file
/// @brief Implementation of helper functions for testing draw_ui_elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "draw_ui_elements_helpers.h"
#include "catch2/catch_test_macros.hpp"
#include <SFML/Graphics/Image.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
UIStyle CreateTestUIStyle() {
  UIStyle style;
  style.name = "test_style";
  style.panel_style.background_color = sf::Color::Green;
  style.panel_style.border_color = sf::Color::Red;
  style.panel_style.border_thickness = 5.0f;
  style.panel_style.radius_resolution = 10;
  style.panel_style.inner_margin = {10.0f, 10.0f};
  style.panel_style.minimum_size = {20.0f, 20.0f};
  style.panel_style.maximum_size = {200.0f, 200.0f};
  return style;
}
/////////////////////////////////////////////////
void TestDrawBoxWithBorder(const sf::Image &image, const Style &base_style,
                           const sf::Vector2f &position,
                           const sf::Vector2f &size) {
  // Check the border pixels
  for (int x = static_cast<int>(position.x);
       x < static_cast<int>(position.x + size.x); x++) {
    for (int y = static_cast<int>(position.y);
         y < static_cast<int>(position.y + size.y); y++) {
      bool is_border =
          x < static_cast<int>(position.x + base_style.border_thickness) ||
          x >= static_cast<int>(position.x + size.x -
                                base_style.border_thickness) ||
          y < static_cast<int>(position.y + base_style.border_thickness) ||
          y >= static_cast<int>(position.y + size.y -
                                base_style.border_thickness);

      sf::Color pixel = image.getPixel(
          {static_cast<unsigned int>(x), static_cast<unsigned int>(y)});
      // Add CAPTURE for all relevant values
      CAPTURE(x, y, is_border, pixel, base_style.border_color,
              base_style.background_color);

      if (is_border) {
        // Pixel should be border color
        REQUIRE(pixel == base_style.border_color);
      } else {
        // Pixel should be background color
        REQUIRE(pixel == base_style.background_color);
      }
    }
  }
}
/////////////////////////////////////////////////
void TestDrawPanel(sf::Image &image, const PanelElement &panel,
                   const UIStyle &style) {

  // test that the correct pixels are drawn
  TestDrawBoxWithBorder(image, style.panel_style, panel.position, panel.size);
}
} // namespace steamrot::tests
