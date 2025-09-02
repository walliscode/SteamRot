/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions for drawing UI elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "draw_ui_elements.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cstdint>

namespace steamrot {
namespace draw_ui_elements {

/////////////////////////////////////////////////
void DrawText(sf::RenderTexture &texture, const std::string &text,
              const sf::Vector2f &position, const sf::Vector2f size,
              std::shared_ptr<const sf::Font> font, uint8_t font_size,
              const sf::Color &color) {

  // create the text object
  sf::Text text_object(*font, text, font_size);

  // set the fill color
  text_object.setFillColor(color);
  // set the origin to the center of the text
  text_object.setOrigin(text_object.getLocalBounds().getCenter());

  // calculate the center of the container
  sf::Vector2f container_center = sf::FloatRect(position, size).getCenter();
  // set the position of the text to the center of the container
  text_object.setPosition(container_center);

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

  DrawText(texture, button.label, text_position, button.size,
           style.button_style.font, style.button_style.font_size,
           style.button_style.text_color);
}
} // namespace draw_ui_elements
} // namespace steamrot
