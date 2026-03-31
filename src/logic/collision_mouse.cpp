/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for mouse collision checking of
/// MachinaFormScaffold parts
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_mouse.h"

namespace steamrot::logic::collision::mouse {

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
