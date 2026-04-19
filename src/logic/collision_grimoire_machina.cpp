/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for collision checking of
/// GrimoireMachina elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_grimoire_machina.h"
#include <cmath>

namespace steamrot::logic::collision::grimoire_machina {

/////////////////////////////////////////////////
void check_socket_collisions(SocketData &socket_data,
                             const sf::Transform &socket_transform,
                             SocketData &other_socket_data,
                             const sf::Transform &other_socket_transform) {

  // collision variables baked into the logic
  static constexpr float proximity_distance_threshold = 10.f;
  static constexpr float connection_distance_threshold = 2.5f;
  // calculate the world position of each socket
  sf::Vector2f socket_world_position =
      socket_transform.transformPoint(socket_data.local_position);
  sf::Vector2f other_socket_world_position =
      other_socket_transform.transformPoint(other_socket_data.local_position);

  // calculate the distance between the two sockets
  float distance_between_sockets =
      std::hypot(socket_world_position.x - other_socket_world_position.x,
                 socket_world_position.y - other_socket_world_position.y);

  // check if the sockets are within the connection threshold
  if (distance_between_sockets <= connection_distance_threshold) {
    // set ready to connect state for both sockets
    socket_data.is_ready_to_connect = true;
    socket_data.is_another_socket_near = true;
    other_socket_data.is_ready_to_connect = true;
    other_socket_data.is_another_socket_near = true;

    // else if they are within the proximity threshold but not close enough to
    // connect
  } else if (distance_between_sockets <= proximity_distance_threshold) {
    // set proximity state for both sockets
    socket_data.is_another_socket_near = true;
    socket_data.is_ready_to_connect = false;
    other_socket_data.is_another_socket_near = true;
    socket_data.is_ready_to_connect = false;

  } else {
    // reset proximity state if they are outside the threshold
    socket_data.is_another_socket_near = false;
    socket_data.is_ready_to_connect = false;
    other_socket_data.is_another_socket_near = false;
    other_socket_data.is_ready_to_connect = false;
  }
  return;
}
/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             PartMap &part_map) {}
} // namespace steamrot::logic::collision::grimoire_machina
