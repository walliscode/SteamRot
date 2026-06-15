/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
void initialize_joint_socket_positions(JointInstance &instance) {
  if (!instance.joint)
    return;

  std::vector<sf::Vector2f> positions;
  compute_socket_local_positions_even_spread(instance.joint->socket_config,
                                             instance.joint->origin, positions);

  for (auto &[socket_id, socket] : instance.sockets) {
    socket.local_position = positions[socket_id];
  }
}

/////////////////////////////////////////////////
void compute_socket_local_positions_even_spread(
    const SocketConfig &config, const sf::Vector2f &origin,
    std::vector<sf::Vector2f> &local_positions) {

  // if no sockets, return early (also avoids divide by zero in later
  // calculations)
  if (config.socket_count == 0) {
    return;
  }
  // extra guard statement to prevent undefined access
  local_positions.clear();
  local_positions.resize(config.socket_count);

  // calculate the angle at which to place the socket(s) using the socket
  // config's arc min and mx and socket count
  const float arc_min = config.rotation_arc_min;
  const float arc_max = config.rotation_arc_max;
  const float arc_range = arc_max - arc_min;

  // for a single socket, place it at the midpoint of the arc
  if (config.socket_count == 1) {
    const float angle_deg = arc_min + arc_range / 2.f;
    const float angle_rad = sf::degrees(angle_deg).asRadians();
    local_positions[0] =
        origin +
        sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} * config.radius;
  } else {
    const float angle_between_sockets = arc_range / (config.socket_count - 1);
    // for greater that one socket, place them on the arc min/max range, evenly
    // spaced
    for (size_t i = 0; i < config.socket_count; ++i) {
      const float angle_deg = arc_min + (i * angle_between_sockets);
      const float angle_rad = sf::degrees(angle_deg).asRadians();
      local_positions[i] =
          origin + sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} *
                       config.radius;
    }
  }
  return;
}

/////////////////////////////////////////////////
void position_first_part_of_machina_form_scaffold(PartGraph &parts) {
  // check that parts is not empty, if not return early
  if (parts.empty()) {
    return;
  }

  // if part 0 does not exist, return early
  auto it = parts.find(0);
  if (it == parts.end()) {
    return;
  }

  // get variant
  if (FragmentInstance *fragment_instance =
          std::get_if<FragmentInstance>(&it->second)) {

    // if views is empty, return early
    if (fragment_instance->fragment->positioning_views.empty())
      return;

    // working off the FRONT view only, get the center of the bounding box of
    // the FRONT view's vertex array
    const sf::VertexArray &va =
        fragment_instance->fragment->positioning_views[ViewDirection::Front];
    // get centre using built in sfml functions
    sf::Vector2f center = va.getBounds().getCenter();

    // reset the transform of the fragment instance to identity
    fragment_instance->transform = sf::Transform::Identity;
    // apply the offset to the transform of the fragment instance
    fragment_instance->transform.translate(-center);

    return;
  }

  if (JointInstance *joint_instance = std::get_if<JointInstance>(&it->second)) {
    // if views is empty, return early
    if (joint_instance->joint->positioning_views.empty())
      return;

    // reset the transform of the joint instance to identity
    joint_instance->transform = sf::Transform::Identity;
    // we work of the origin of the joint for positioning
    // most Joints are likely to be set at 0,0 when creating, but we should
    // still account for the possibility of an offset
    joint_instance->transform.translate(-joint_instance->joint->origin);

    // populate socket positions now that the joint is placed
    initialize_joint_socket_positions(*joint_instance);

    return;
  }
}

/////////////////////////////////////////////////
void position_machina_form_scaffold(PartGraph &parts) {
  // if parts is empty, return early
  if (parts.empty())
    return;

  // position the first part of the scaffold at 0,0
  position_first_part_of_machina_form_scaffold(parts);
}
} // namespace steamrot::logic::positioning::grimoire_machina
