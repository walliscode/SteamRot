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
sf::Vector2f compute_socket_local_pos(const SocketConfig &config,
                                      size_t socket_index,
                                      float ring_rotation) {
  // Fixed anchor socket: sits at fixed_socket_angle and does not rotate
  if (config.has_fixed_socket && socket_index == 0) {
    const float rad = sf::degrees(config.fixed_socket_angle).asRadians();
    return {config.radius * std::cos(rad), config.radius * std::sin(rad)};
  }

  // Offset index to account for fixed socket occupying slot 0
  const size_t rot_index =
      config.has_fixed_socket ? socket_index - 1 : socket_index;
  const size_t rot_count =
      config.has_fixed_socket
          ? static_cast<size_t>(config.socket_count) - 1
          : static_cast<size_t>(config.socket_count);

  float base_angle_deg;
  if (rot_count <= 1) {
    base_angle_deg = (config.arc_min + config.arc_max) / 2.f;
  } else {
    const float span = config.arc_max - config.arc_min;
    // Full circle: avoid duplicating the start/end position
    if (std::abs(span - 360.f) < 1e-3f) {
      base_angle_deg = config.arc_min + static_cast<float>(rot_index) *
                                            360.f /
                                            static_cast<float>(rot_count);
    } else {
      base_angle_deg = config.arc_min + static_cast<float>(rot_index) * span /
                                            static_cast<float>(rot_count - 1);
    }
  }

  const float rad = sf::degrees(base_angle_deg + ring_rotation).asRadians();
  return {config.radius * std::cos(rad), config.radius * std::sin(rad)};
}

/////////////////////////////////////////////////
void initialize_joint_socket_positions(JointInstance &joint_instance) {
  const auto &config = joint_instance.joint.socket_config;
  const size_t count = static_cast<size_t>(config.socket_count);
  joint_instance.socket_local_positions.resize(count);
  for (size_t i = 0; i < count; ++i) {
    joint_instance.socket_local_positions[i] =
        compute_socket_local_pos(config, i, 0.f);
  }
}

/////////////////////////////////////////////////
void position_first_part_of_machina_form_scaffold(PartMap &parts) {
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
    if (fragment_instance->fragment.movement_views.empty())
      return;

    // working off the FRONT view only, get the center of the bounding box of
    // the FRONT view's vertex array
    const sf::VertexArray &va =
        fragment_instance->fragment.movement_views[ViewDirection::Front];
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
    if (joint_instance->joint.movement_views.empty())
      return;

    // reset the transform of the joint instance to identity
    joint_instance->transform = sf::Transform::Identity;
    // we work of the origin of the joint for positioning
    // most Joints are likely to be set at 0,0 when creating, but we should
    // still account for the possibility of an offset
    joint_instance->transform.translate(-joint_instance->joint.origin);

    // initialise socket positions now that the transform is established
    initialize_joint_socket_positions(*joint_instance);

    return;
  }
}

/////////////////////////////////////////////////
void position_machina_form_scaffold(PartMap &parts) {
  // if parts is empty, return early
  if (parts.empty())
    return;

  // position the first part of the scaffold at 0,0
  position_first_part_of_machina_form_scaffold(parts);
}
} // namespace steamrot::logic::positioning::grimoire_machina
