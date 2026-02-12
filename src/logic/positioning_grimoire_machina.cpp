/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
sf::FloatRect CalculateCraftingCanvasSizeAndPosition(
    const sf::FloatRect &texture_coordinates,
    const std::vector<CUserInterface> &crafting_ui_elements) {
  // create return rectangle shape with the same size and position as the
  // texture coordinates
  sf::FloatRect crafting_canvas = texture_coordinates;

  // iterate through crafting UI elements and reduce the size and position of
  // the crafting canvas by the size and position of the crafting UI elements
  for (const CUserInterface &ui_element : crafting_ui_elements) {

    // only consider visible crafting UI elements
    if (!ui_element.m_visible)
      continue;

    // the size of the crafting canvas is reduced by the size of the crafting UI
    // only if it overlaps with the crafting canvas
    if (ui_element.m_root_element) {

      // create a rectangle from the parent element of the crafting UI element
      const sf::FloatRect ui_element_rect{ui_element.m_root_element->position,
                                          ui_element.m_root_element->size};

      // check if the crafting canvas overlaps with the crafting UI element
      if (auto intersection =
              crafting_canvas.findIntersection(ui_element_rect)) {

        // if overlap then move the x position to the edge of the crafting UI
        // element
        crafting_canvas.position.x =
            ui_element_rect.position.x + ui_element_rect.size.x;
        // size will be what remains of the texture cooridinates
        crafting_canvas.size.x =
            texture_coordinates.size.x -
            (crafting_canvas.position.x - texture_coordinates.position.x);
      }
    }
  }
  return crafting_canvas;
}

/////////////////////////////////////////////////
void PositionGrowthPoint(GrowthPoint &growth_point,
                         const sf::FloatRect &crafting_canvas) {
  // position the growth point in the center of the crafting canvas
  growth_point.origin.setPosition(
      {crafting_canvas.position.x + crafting_canvas.size.x / 2.f,
       crafting_canvas.position.y + crafting_canvas.size.y / 2.f});
}
} // namespace steamrot::logic::positioning::grimoire_machina
