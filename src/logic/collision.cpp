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

/////////////////////////////////////////////////
void CheckMouseOverNestedUIElement(const sf::Vector2i &mouse_position,
                                   UIElement &element) {
  // bool to keep track if any child is hovered over
  bool child_hovered = false;

  // cycle through all child elements and check if any are hovered
  for (auto &child : element.child_elements) {

    // go as deep as possible first, this will stop when no children are
    // detected
    CheckMouseOverNestedUIElement(mouse_position, *child);
    if (child->is_mouse_over) {
      // for the parent to evaluate
      child_hovered = true;
      // if a child is hovered, no need to check further children
      break;
    }
  }
  if (child_hovered) {
    // if a child is hovered, parent cannot be hovered
    element.is_mouse_over = false;
  } else {
    // this will occur if no child is hovered (or no children exist)
    CheckMouseOverUIElement(mouse_position, element);
  }
}
} // namespace collision
} // namespace steamrot
