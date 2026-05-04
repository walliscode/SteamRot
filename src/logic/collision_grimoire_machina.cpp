/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for collision checking of
/// GrimoireMachina elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_grimoire_machina.h"
#include <algorithm>
#include <cmath>
#include <variant>

namespace steamrot::logic::collision::grimoire_machina {

namespace {

static constexpr float k_proximity_distance_threshold = 10.f;
static constexpr float k_connection_distance_threshold = 2.5f;

/////////////////////////////////////////////////
/// @brief Reset proximity state on a single SocketData.
/////////////////////////////////////////////////
void reset_socket(SocketData &socket) {
  socket.is_another_socket_near = false;
  socket.is_ready_to_connect = false;
  socket.distance_to_nearest_socket = std::nullopt;
  socket.proximity_scale = std::nullopt;
}

/////////////////////////////////////////////////
/// @brief Compute the proximity brightness scale [0, 255] from a distance.
///
/// Maps @p distance from the range
/// [@c k_connection_distance_threshold, @c k_proximity_distance_threshold]
/// onto [255, 0]: 255 when at or inside the connection threshold, 0 when at
/// the outer proximity boundary.
///
/// @param distance World-space distance to the candidate partner socket.
/// @return Scale value in [0, 255].
/////////////////////////////////////////////////
uint8_t compute_proximity_scale(float distance) {
  constexpr float range =
      k_proximity_distance_threshold - k_connection_distance_threshold;
  static_assert(range > 0.f,
                "proximity threshold must be strictly greater than connection "
                "threshold");
  const float t = (k_proximity_distance_threshold - distance) / range;
  const float clamped = std::clamp(t, 0.f, 1.f);
  return static_cast<uint8_t>(clamped * 255.f);
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
  socket.proximity_scale = compute_proximity_scale(distance);
}

} // namespace

/////////////////////////////////////////////////
void reset_socket_proximity_state(PartGraph &part_graph) {
  for (auto &[id, variant] : part_graph) {
    if (auto *fi = std::get_if<FragmentInstance>(&variant)) {
      for (auto &[socket_id, socket_data] : fi->sockets)
        reset_socket(socket_data);
    } else if (auto *ji = std::get_if<JointInstance>(&variant)) {
      for (auto &[socket_id, socket_data] : ji->sockets)
        reset_socket(socket_data);
    }
  }
}

/////////////////////////////////////////////////
void check_socket_collisions(SocketData &socket_data,
                             const sf::Transform &socket_transform,
                             SocketData &other_socket_data,
                             const sf::Transform &other_socket_transform) {

  // return early if either socket is not available
  if (socket_data.state != SocketState::Available ||
      other_socket_data.state != SocketState::Available) {
    return;
  }

  const sf::Vector2f socket_world_pos =
      socket_transform.transformPoint(socket_data.local_position);
  const sf::Vector2f other_socket_world_pos =
      other_socket_transform.transformPoint(other_socket_data.local_position);

  const float distance =
      std::hypot(socket_world_pos.x - other_socket_world_pos.x,
                 socket_world_pos.y - other_socket_world_pos.y);

  if (distance > k_proximity_distance_threshold)
    return; // outside both thresholds — no update

  const bool ready = distance <= k_connection_distance_threshold;
  apply_if_better(socket_data, distance, ready);
  apply_if_better(other_socket_data, distance, ready);
}

/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             JointInstance &joint_instance) {
  for (auto &[fragment_socket_id, fragment_socket] :
       fragment_instance.sockets) {
    for (auto &[joint_socket_id, joint_socket] : joint_instance.sockets) {
      check_socket_collisions(fragment_socket, fragment_instance.transform,
                              joint_socket, joint_instance.transform);
    }
  }
}

/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             PartGraph &part_graph) {
  // Reset state on both sides before each pass so that stale state from the
  // previous tick does not bleed through.
  for (auto &[socket_id, socket_data] : fragment_instance.sockets)
    reset_socket(socket_data);
  reset_socket_proximity_state(part_graph);

  for (auto &[id, variant] : part_graph) {
    if (auto *joint_instance = std::get_if<JointInstance>(&variant)) {
      check_socket_collisions(fragment_instance, *joint_instance);
    }
  }
}

/////////////////////////////////////////////////
void check_socket_collisions(JointInstance &joint_instance, PartGraph &part_graph) {
  // Reset state on both sides before each pass so that stale state from the
  // previous tick does not bleed through.
  for (auto &[socket_id, socket_data] : joint_instance.sockets)
    reset_socket(socket_data);
  reset_socket_proximity_state(part_graph);

  for (auto &[id, variant] : part_graph) {
    if (auto *fragment_instance = std::get_if<FragmentInstance>(&variant)) {
      check_socket_collisions(*fragment_instance, joint_instance);
    }
  }
}

/////////////////////////////////////////////////
void check_collisions_between_ghost_and_scaffold(
    MachinaFormScaffold &scaffold_form, MrGhost &mr_ghost) {

  // guard statement for monostate
  if (std::holds_alternative<std::monostate>(mr_ghost.m_instance)) {
    return; // no active ghost item, nothing to check
  }

  // Check for collisions between the active ghost item and the active scaffold.
  if (auto *fragment_instance =
          std::get_if<FragmentInstance>(&mr_ghost.m_instance)) {
    check_socket_collisions(*fragment_instance, scaffold_form.parts);
  } else if (auto *joint_instance =
                 std::get_if<JointInstance>(&mr_ghost.m_instance)) {
    check_socket_collisions(*joint_instance, scaffold_form.parts);
  }
}
} // namespace steamrot::logic::collision::grimoire_machina
