/////////////////////////////////////////////////
/// @file
/// @brief Implementation of collision detection functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision.h"

namespace steamrot {
namespace collision {

/////////////////////////////////////////////////
bool IsMouseOverBounds(const sf::Vector2i &mouse_position,
                       const sf::FloatRect &bounds) {

  return (bounds.contains(sf::Vector2f(mouse_position)));
}

/////////////////////////////////////////////////
void CheckMouseOverUIElement(const sf::Vector2i &mouse_position,
                             UIElement &element) {
  if (IsMouseOverBounds(mouse_position,
                        sf::FloatRect(element.position, element.size))) {
    element.is_mouse_over = true;
  } else {
    element.is_mouse_over = false;
  }
}
} // namespace collision
} // namespace steamrot
