/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include <SFML/Graphics/RectangleShape.hpp>

namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
sf::RectangleShape CalculateCraftingCanvasSizeAndPosition(
    const sf::RectangleShape &texture_coordinates,
    const std::vector<CUserInterface> &crafting_ui_elements) {
  // create return rectangle shape with the same size and position as the
  // texture coordinates
  sf::RectangleShape crafting_canvas = texture_coordinates;

  return crafting_canvas;
}

} // namespace steamrot::logic::positioning::grimoire_machina
