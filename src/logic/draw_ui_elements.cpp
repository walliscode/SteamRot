/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions for drawing UI elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "draw_ui_elements.h"
#include <SFML/Graphics/Text.hpp>
#include <cstdint>
#include <iostream>

namespace steamrot {
namespace draw_ui_elements {

/////////////////////////////////////////////////
void DrawText(sf::RenderTexture &texture, const std::string &text,
              const sf::Vector2f &position,
              std::shared_ptr<const sf::Font> font, uint8_t font_size,
              const sf::Color &color) {

  sf::Text text_object{*font, text, font_size};
  text_object.setFillColor(color);
  text_object.setPosition(position);

  std::cout << "Drawing text: '" << text << "' at position (" << position.x
            << ", " << position.y << ") with font size "
            << static_cast<int>(font_size) << std::endl;
  texture.draw(text_object);
}
/////////////////////////////////////////////////
void DrawUIElement(sf::RenderTexture &texture, const PanelElement &panel,
                   const UIStyle &style) {

  // Create the panel using the style provided
  sf::RectangleShape rectangle(panel.size);
  rectangle.setPosition(panel.position);
  rectangle.setFillColor(style.panel_style.background_color);
  rectangle.setOutlineColor(style.panel_style.border_color);
  // Border thickness is negative to draw inwards
  rectangle.setOutlineThickness(-style.panel_style.border_thickness);

  // Draw the rectangle on the texture
  texture.draw(rectangle);
}

/////////////////////////////////////////////////
void DrawUIElement(sf::RenderTexture &texture, const ButtonElement &button,
                   const UIStyle &style) {
  // Create the button using the style provided
  sf::RectangleShape rectangle(button.size);
  rectangle.setPosition(button.position);
  rectangle.setFillColor(style.button_style.background_color);
  rectangle.setOutlineColor(style.button_style.border_color);
  // Border thickness is negative to draw inwards
  rectangle.setOutlineThickness(-style.button_style.border_thickness);
  // Draw the rectangle on the texture
  texture.draw(rectangle);
  // Draw the button text
  sf::Vector2f text_position{
      button.position.x + style.button_style.border_thickness +
          style.button_style.inner_margin.x,
      button.position.y + style.button_style.border_thickness +
          style.button_style.inner_margin.y};
  DrawText(texture, button.label, text_position, style.button_style.font,
           style.button_style.font_size, style.button_style.text_color);
}
} // namespace draw_ui_elements
} // namespace steamrot
