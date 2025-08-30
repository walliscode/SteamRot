/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions for drawing UI elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "draw_ui_elements.h"
#include <SFML/Graphics/Text.hpp>

namespace steamrot {
namespace draw_ui_elements {

/////////////////////////////////////////////////
void DrawText(sf::RenderTexture &texture, const std::string &text,
              const sf::Vector2f &position, const UIStyle &style) {
  // Placeholder function for drawing text

  // In a real implementation, you would use sf::Text and set its font, size,
  // color, etc.
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
}
} // namespace draw_ui_elements
} // namespace steamrot
