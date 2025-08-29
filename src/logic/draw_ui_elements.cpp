/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions for drawing UI elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "draw_ui_elements.h"

namespace steamrot {
namespace draw_ui_elements {

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
} // namespace draw_ui_elements
} // namespace steamrot
