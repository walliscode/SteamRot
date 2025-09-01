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
#include <SFML/System/Vector2.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
void TestDrawBoxWithBorder(const sf::Image &image, const Style &base_style,
                           const sf::Vector2f &position,
                           const sf::Vector2f &size, bool test_inner_area) {
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
        // only test inner area if specified
        if (test_inner_area)
          REQUIRE(pixel == base_style.background_color);
      }
    }
  }
}
/////////////////////////////////////////////////
void TestTextIsPresent(const sf::Image &image, const sf::Vector2f &position,
                       const sf::Vector2f &size, const sf::Color &text_color) {
  bool found_text_pixel = false;
  for (int x = static_cast<int>(position.x);
       x < static_cast<int>(position.x + size.x); x++) {
    for (int y = static_cast<int>(position.y);
         y < static_cast<int>(position.y + size.y); y++) {
      sf::Color pixel = image.getPixel(
          {static_cast<unsigned int>(x), static_cast<unsigned int>(y)});
      if (pixel == text_color) {
        found_text_pixel = true;
      }
    }
  }

  REQUIRE(found_text_pixel == true);
}

/////////////////////////////////////////////////
void TestDrawPanel(sf::Image &image, const PanelElement &panel,
                   const UIStyle &style) {

  // test that the correct pixels are drawn
  TestDrawBoxWithBorder(image, style.panel_style, panel.position, panel.size,
                        true);
}

/////////////////////////////////////////////////
void TestDrawButton(sf::Image &image, const ButtonElement &button,
                    const UIStyle &style) {
  // test that button box with border is drawn correctly
  TestDrawBoxWithBorder(image, style.button_style, button.position,
                        button.size);

  // create inner section
  sf::Vector2f inner_position{
      (button.position.x + style.button_style.border_thickness +
       style.button_style.inner_margin.x),
      (button.position.y + style.button_style.border_thickness +
       style.button_style.inner_margin.y)};
  sf::Vector2f inner_size{
      button.size.x - 2 * style.button_style.border_thickness -
          2 * style.button_style.inner_margin.x,
      button.size.y - 2 * style.button_style.border_thickness -
          2 * style.button_style.inner_margin.y};

  TestTextIsPresent(image, inner_position, inner_size,
                    style.button_style.text_color);
}
} // namespace steamrot::tests
