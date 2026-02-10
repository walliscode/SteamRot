/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions related to the rendering of the
/// grimoire
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_grimoire_machina.h"
#include <SFML/Graphics/RectangleShape.hpp>

namespace steamrot::logic::render::grimoire_machina {
/////////////////////////////////////////////////
void DrawEmptyActiveMachinaForm(sf::RenderTexture &texture, MachinaForm &form) {
}

/////////////////////////////////////////////////
void DrawCraftingCanvasBorder(sf::RenderTexture &texture,
                              sf::FloatRect &crafting_canvas) {

  // create a rectangle shape for the border
  sf::RectangleShape border;
  float border_thickness = 3.f; // thickness of the border
  // off set border by thicknes so that the border remains "inside" the crafting
  // canvas
  border.setPosition({crafting_canvas.position.x + border_thickness,
                      crafting_canvas.position.y + border_thickness});
  border.setSize({crafting_canvas.size.x - 2 * border_thickness,
                  crafting_canvas.size.y - 2 * border_thickness});

  border.setFillColor(sf::Color::Transparent);
  border.setOutlineColor(sf::Color::White);
  border.setOutlineThickness(border_thickness);

  texture.draw(border);
}
} // namespace steamrot::logic::render::grimoire_machina
