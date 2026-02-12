/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for collision handling of Grimoire
/// Machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_grimoire_machina.h"
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::collision::grimoire_machina {

/////////////////////////////////////////////////
void CheckMouseOverGrowthPoint(const sf::Vector2i &mouse_position,
                               GrowthPoint &growth_point) {

  // Check if the mouse is over the growth point origin
  if (growth_point.origin.getGlobalBounds().contains(
          sf::Vector2f{mouse_position})) {
    growth_point.is_mouse_over = true;
  } else {
    growth_point.is_mouse_over = false;
  }
}
} // namespace steamrot::logic::collision::grimoire_machina
