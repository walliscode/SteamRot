/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the JointInstance class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointInstance.h"
#include <cmath>
#include <iostream>

namespace steamrot {

/////////////////////////////////////////////////
JointInstance::JointInstance(
    const Joint *joint_ptr,
    const JointSocketPositioningStrategy positioning_strategy,
    sf::Transform initial_transform)
    : PartInstance{initial_transform}, joint{joint_ptr},
      socket_pivot{joint_ptr ? joint_ptr->socket_pivot
                             : sf::Vector2f{0.f, 0.f}} {

  // If the joint pointer is null, we cannot initialize sockets, so we return
  // early.
  if (!joint_ptr)
    return;

  // Initialize the sockets map with default JointSocketState for each socket
  // index.
  for (uint32_t i = 0;
       i < static_cast<uint32_t>(joint_ptr->socket_config.socket_count); ++i)
    sockets.emplace(i, JointSocketState{});

  // Position the sockets according to the specified strategy.
  PositionSockets(positioning_strategy);
}

/////////////////////////////////////////////////
void JointInstance::SetConnection(const SocketConnection &connection,
                                  const uint32_t socket_id) {
  auto socket_it = sockets.find(socket_id);
  if (socket_it != sockets.end()) {
    socket_it->second.SetConnection(connection);
  }
}
/////////////////////////////////////////////////
void JointInstance::PositionSockets(
    const JointSocketPositioningStrategy positioning_strategy) {}

/////////////////////////////////////////////////
sf::Vector2f JointInstance::CalculateAlignmentVectorWorld(
    const uint32_t joint_socket_id) const {
  // check that the joint socket id is valid
  auto socket_it = sockets.find(joint_socket_id);
  if (socket_it == sockets.end())
    return sf::Vector2f{0.f, 0.f};

  // generate transform from the total rotation of the joint instance
  sf::Transform rotation_transform;
  rotation_transform.rotate(total_rotation);

  // generate a local alignment vector from the joint socket pivot to the joint
  // socket local position
  sf::Vector2f local_alignment_vector =
      socket_it->second.GetLocalPosition() - joint->socket_pivot;

  // apply the rotation transform to the local alignment vector to give the
  // world alignment vector
  sf::Vector2f alignment_vector =
      rotation_transform.transformPoint(local_alignment_vector);

  // return the normalized alignment vector
  return alignment_vector.normalized();
}

/////////////////////////////////////////////////
const sf::Vector2f
JointInstance::GetSocketWorldPosition(const uint32_t socket_id) const {
  auto socket_it = sockets.find(socket_id);
  if (socket_it != sockets.end()) {
    return transform.transformPoint(socket_it->second.GetLocalPosition());
  }
  return sf::Vector2f{0.f, 0.f};
}

/////////////////////////////////////////////////
const sf::Vector2f
JointInstance::GetSocketWorldAlignmentVector(const uint32_t socket_id) const {
  auto socket_it = sockets.find(socket_id);
  if (socket_it != sockets.end()) {
    return CalculateAlignmentVectorWorld(socket_id);
  }
  return sf::Vector2f{0.f, 0.f};
}

/////////////////////////////////////////////////
const JointSocketState &
JointInstance::GetSocket(const uint32_t socket_id) const {
  auto socket_it = sockets.find(socket_id);
  if (socket_it != sockets.end()) {
    return socket_it->second;
  }
  throw std::out_of_range("Socket ID not found in JointInstance.");
}
/////////////////////////////////////////////////
const bool JointInstance::GetMouseOverSocketState(uint32_t socket_id) const {
  auto socket_it = sockets.find(socket_id);
  if (socket_it != sockets.end()) {
    return socket_it->second.IsMouseOver();
  }
  return false;
}

/////////////////////////////////////////////////
void JointInstance::CheckMouseOver(const sf::Vector2f &world_mouse) {
  for (auto &[socket_id, socket] : sockets) {
    // get the world position of the socket by applying the joint's transform
    // to the stored local socket position
    const sf::Vector2f world_pos =
        transform.transformPoint(socket.GetLocalPosition());

    // check if the mouse is over this socket and update the socket state
    // accordingly
    socket.CheckMouseOver(world_mouse, world_pos);
  }
}
/// FREE FUNCTION(S) /// //////////////////////////////////////////////
void maximise_joint_socket_spread(JointSockets &joint_sockets,
                                  const sf::Vector2f &pivot,
                                  const SocketConfig &config) {

  const float arc_min = config.rotation_arc_min;
  const float arc_max = config.rotation_arc_max;
  const float arc_range = arc_max - arc_min;

  // if socket count is 0, return early
  if (config.socket_count == 0) {
    return;
  }

  // if socket count is 1, place it at the midpoint of the arc
  if (config.socket_count == 1) {
    const float angle_deg = config.rotation_arc_min + arc_range / 2.f;
    const float angle_rad = sf::degrees(angle_deg).asRadians();

    const sf::Vector2f new_local_position =
        pivot +
        sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} * config.radius;
    joint_sockets.at(0).SetLocalPosition(new_local_position);

    // for greater that one socket, place them on the arc min/max range and then
    // evenly spread them out across the arc

  } else {
    const float angle_between_sockets = arc_range / (config.socket_count - 1);
    for (size_t i = 0; i < config.socket_count; ++i) {
      const float angle_deg = arc_min + (i * angle_between_sockets);
      const float angle_rad = sf::degrees(angle_deg).asRadians();
      const sf::Vector2f new_local_position =
          pivot + sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} *
                      config.radius;
      joint_sockets.at(i).SetLocalPosition(new_local_position);
    }
  }
}

/////////////////////////////////////////////////
bool check_if_allowed_joint_socket_configuration(
    const JointSockets &joint_sockets, const sf::Vector2f &pivot,
    const SocketConfig &config) {

  constexpr float kPi = 3.14159265358979323846f;
  constexpr float kRadToDeg = 180.0f / kPi;

  auto normalize_degrees_0_360 = [](float deg) -> float {
    float normalized = std::fmod(deg, 360.0f);
    if (normalized < 0.0f)
      normalized += 360.0f;
    return normalized;
  };

  auto angle_degrees_from_position =
      [&](const sf::Vector2f &position) -> float {
    sf::Vector2f dir = position - pivot;
    float angle_rad = std::atan2(dir.y, dir.x);
    float angle_deg = angle_rad * kRadToDeg;
    return normalize_degrees_0_360(angle_deg);
  };

  auto is_angle_within_arc = [&](float angle_deg, float arc_min_deg,
                                 float arc_max_deg) -> bool {
    float min_norm = normalize_degrees_0_360(arc_min_deg);
    float max_norm = normalize_degrees_0_360(arc_max_deg);

    // Non-wrapping arc: [min, max]
    if (min_norm <= max_norm) {
      return angle_deg >= min_norm && angle_deg <= max_norm;
    }

    // Wrapping arc (e.g. 300..30): [min..360) U [0..max]
    return angle_deg >= min_norm || angle_deg <= max_norm;
  };

  auto smallest_angular_difference_degrees = [&](float a_deg,
                                                 float b_deg) -> float {
    float diff = std::abs(a_deg - b_deg);
    return std::min(diff, 360.0f - diff);
  };

  // check whether the local positions fall within the arc defined by the socket
  // config
  for (const auto &[socket_id, socket] : joint_sockets) {

    sf::Vector2f direction = socket.GetLocalPosition() - pivot;
    float distance = direction.length();
    float radius_error = std::abs(distance - config.radius);

    float angle_deg = angle_degrees_from_position(socket.GetLocalPosition());

    // check distance to pivot matches configured radius
    if (radius_error > 0.01f) {
      return false;
    }

    // check whether angle is inside configured arc
    if (!is_angle_within_arc(angle_deg, config.rotation_arc_min,
                             config.rotation_arc_max)) {
      return false;
    }

    // check minimum angular gap against all other sockets
    for (const auto &[other_socket_id, other_socket] : joint_sockets) {
      if (socket_id == other_socket_id)
        continue;

      float other_angle_deg =
          angle_degrees_from_position(other_socket.GetLocalPosition());

      float angle_difference =
          smallest_angular_difference_degrees(angle_deg, other_angle_deg);

      std::cerr << "[SocketCheck] pair=(" << socket_id << "," << other_socket_id
                << ")"
                << " angle_a=" << angle_deg << ", angle_b=" << other_angle_deg
                << ", diff=" << angle_difference
                << ", min_gap=" << config.minimum_gap << "\n";

      if (angle_difference < config.minimum_gap) {
        std::cerr << "[SocketCheck] FAIL(pair=" << socket_id << ","
                  << other_socket_id
                  << "): angular gap too small. diff=" << angle_difference
                  << ", minimum_gap=" << config.minimum_gap << "\n";
        return false;
      }
    }
  }

  std::cerr << "[SocketCheck] PASS: configuration allowed\n";
  return true;
}
} // namespace steamrot
