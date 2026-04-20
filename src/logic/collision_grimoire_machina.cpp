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
#include <variant>

namespace steamrot::logic::collision::grimoire_machina {

namespace {

/////////////////////////////////////////////////
/// @brief Reset proximity state on a single SocketData.
/////////////////////////////////////////////////
void reset_socket(SocketData &socket) {
  socket.is_another_socket_near = false;
  socket.is_ready_to_connect = false;
  socket.distance_to_nearest_socket = std::nullopt;
}

/////////////////////////////////////////////////
/// @brief Write proximity state onto a socket when the new distance is a
///        strictly better (smaller) candidate than the currently stored one.
///
/// @param socket   SocketData to conditionally update.
/// @param distance World-space distance to the candidate partner socket.
/// @param ready    Whether this distance qualifies as "ready to connect".
/////////////////////////////////////////////////
void apply_if_better(SocketData &socket, float distance, bool ready) {
  if (socket.distance_to_nearest_socket.has_value() &&
      distance >= socket.distance_to_nearest_socket.value()) {
    return; // a closer candidate was already recorded
  }
  socket.is_another_socket_near = true;
  socket.is_ready_to_connect = ready;
  socket.distance_to_nearest_socket = distance;
}

} // namespace

/////////////////////////////////////////////////
void reset_socket_proximity_state(PartMap &part_map) {
  for (auto &[id, variant] : part_map) {
    if (auto *fi = std::get_if<FragmentInstance>(&variant)) {
      for (SocketData &s : fi->sockets)
        reset_socket(s);
    } else if (auto *ji = std::get_if<JointInstance>(&variant)) {
      for (SocketData &s : ji->sockets)
        reset_socket(s);
    }
  }
}

/////////////////////////////////////////////////
void check_socket_collisions(SocketData &socket_data,
                             const sf::Transform &socket_transform,
                             SocketData &other_socket_data,
                             const sf::Transform &other_socket_transform) {

  static constexpr float proximity_distance_threshold = 10.f;
  static constexpr float connection_distance_threshold = 2.5f;

  const sf::Vector2f socket_world_pos =
      socket_transform.transformPoint(socket_data.local_position);
  const sf::Vector2f other_socket_world_pos =
      other_socket_transform.transformPoint(other_socket_data.local_position);

  const float distance =
      std::hypot(socket_world_pos.x - other_socket_world_pos.x,
                 socket_world_pos.y - other_socket_world_pos.y);

  if (distance > proximity_distance_threshold)
    return; // outside both thresholds — no update

  const bool ready = distance <= connection_distance_threshold;
  apply_if_better(socket_data, distance, ready);
  apply_if_better(other_socket_data, distance, ready);
}

/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             JointInstance &joint_instance) {
  for (SocketData &fragment_socket : fragment_instance.sockets) {
    for (SocketData &joint_socket : joint_instance.sockets) {
      check_socket_collisions(fragment_socket, fragment_instance.transform,
                              joint_socket, joint_instance.transform);
    }
  }
}

/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             PartMap &part_map) {
  // Reset state on both sides before each pass so that stale state from the
  // previous tick does not bleed through.
  for (SocketData &s : fragment_instance.sockets)
    reset_socket(s);
  reset_socket_proximity_state(part_map);

  for (auto &[id, variant] : part_map) {
    if (auto *joint_instance = std::get_if<JointInstance>(&variant)) {
      check_socket_collisions(fragment_instance, *joint_instance);
    }
  }
}

/////////////////////////////////////////////////
void check_socket_collisions(JointInstance &joint_instance,
                             PartMap &part_map) {
  // Reset state on both sides before each pass so that stale state from the
  // previous tick does not bleed through.
  for (SocketData &s : joint_instance.sockets)
    reset_socket(s);
  reset_socket_proximity_state(part_map);

  for (auto &[id, variant] : part_map) {
    if (auto *fragment_instance = std::get_if<FragmentInstance>(&variant)) {
      check_socket_collisions(*fragment_instance, joint_instance);
    }
  }
}

} // namespace steamrot::logic::collision::grimoire_machina
