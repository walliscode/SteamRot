/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for mouse collision checking of
/// MachinaFormScaffold parts
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_mouse.h"
#include "UIElement.h"

namespace steamrot::logic::collision::mouse {

/////////////////////////////////////////////////
bool IsMouseOverBounds(const sf::Vector2i &mouse_position,
                       const sf::FloatRect &bounds) {
  return bounds.contains(sf::Vector2f(mouse_position));
}

/////////////////////////////////////////////////
void ClearMouseOver(UIElement &element) {
  element.is_mouse_over = false;
  for (auto &child : element.child_elements) {
    ClearMouseOver(*child);
  }
}

/////////////////////////////////////////////////
bool AnyMouseOver(const UIElement &element) {
  if (element.is_mouse_over) {
    return true;
  }
  for (const auto &child : element.child_elements) {
    if (AnyMouseOver(*child)) {
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position, UIElement &element) {
  // bool to keep track if any child is hovered over
  bool child_hovered = false;

  // cycle through all child elements and check if any are hovered
  for (auto &child : element.child_elements) {
    // go as deep as possible first; stops when no children are detected
    CheckMouseOver(mouse_position, *child);
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
    element.is_mouse_over =
        IsMouseOverBounds(mouse_position,
                          sf::FloatRect(element.position, element.size));
  }
}

/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position,
                    GrowthPoint &growth_point) {
  if (growth_point.origin.getGlobalBounds().contains(
          sf::Vector2f{mouse_position})) {
    growth_point.is_mouse_over = true;
  } else {
    growth_point.is_mouse_over = false;
  }
}

/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position, Socket &socket) {
  if (socket.circle.getGlobalBounds().contains(sf::Vector2f{mouse_position})) {
    socket.is_mouse_over = true;
  } else {
    socket.is_mouse_over = false;
  }
}

/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position,
                    FragmentInstance &fragment_instance) {
  for (auto &socket : fragment_instance.sockets) {
    CheckMouseOver(mouse_position, socket);
  }
}

/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position,
                    JointInstance &joint_instance) {
  for (auto &socket : joint_instance.sockets) {
    CheckMouseOver(mouse_position, socket);
  }
}

} // namespace steamrot::logic::collision::mouse
