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
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

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

      if (is_border) {
        // Pixel should be border color
        for (unsigned int x = 0; x < image.getSize().x; x++) {
          for (unsigned int y = 0; y < image.getSize().y; y++) {
          }
        }
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
        std::cout << "Found text pixel at (" << x << ", " << y << ")"
                  << std::endl;
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

/////////////////////////////////////////////////
void DisplayRenderTexture(const sf::RenderTexture &renderTexture) {

  // Retrieve the texture from the render texture

  sf::Sprite sprite(renderTexture.getTexture());
  // have to flip the sprite vertically to display correctly
  // sprite.setScale({1.f, -1.f});
  // sprite.setOrigin({0.f, sprite.getLocalBounds().size.y});
  sf::RenderWindow window(sf::VideoMode({900, 600}),
                          "SFML RenderTexture Display");
  window.setFramerateLimit(60);

  while (window.isOpen()) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
      // add in ctrl + c to close the window
      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter)
          window.close();
      } // if the event is a close event, set the close window flag to true
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }
    window.clear(sf::Color::Black);
    window.draw(sprite);
    window.display();
  }
}

} // namespace steamrot::tests
