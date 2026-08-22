/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the JointInstance class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointInstance.h"
#include "FailInfo.h"
#include "SocketState.h"
#include <SFML/System/Vector2.hpp>
#include <expected>
#include <format>

namespace steamrot {
/////////////////////////////////////////////////
JointInstance::JointInstance(const uint32_t id, const Joint &joint)
    : PartInstance<JointTraits>(id, joint) {
  // initialise the sockets based on the joint's socket config
  for (uint32_t i = 0; i < joint.socket_config.socket_count; ++i) {
    sockets.emplace(i, JointSocketState{});
  }
}

/////////////////////////////////////////////////
JointInstance::JointInstance(const uint32_t id, const Joint &joint,
                             const std::string &alias)
    : PartInstance<JointTraits>(id, alias, joint) {
  // initialise the sockets based on the joint's socket config
  for (uint32_t i = 0; i < joint.socket_config.socket_count; ++i) {
    sockets.emplace(i, JointSocketState{});
  }
}

/////////////////////////////////////////////////
void JointInstance::PositionSockets(
    const JointSocketPositioningStrategy positioning_strategy) {
  switch (positioning_strategy) {
  case JointSocketPositioningStrategy::MaximizeDistance:
    maximise_joint_socket_spread(sockets, part.socket_pivot,
                                 part.socket_config);
    break;
  }
}

/////////////////////////////////////////////////
std::expected<sf::Vector2f, FailInfo>
JointInstance::GetSocketWorldAlignmentVector(uint32_t socket_id) const {

  // if the socket does not exist, return an error
  const JointSocketState *socket = TryGetSocket(socket_id);
  if (!socket) {
    return std::unexpected(
        FailInfo{FailMode::MissingData,
                 std::format("Socket ID {} does not exist.", socket_id)});
  }

  // calculate the local alignment vector going from the socket pivot to the
  // socket's local position
  const sf::Vector2f &local_alignment_vector =
      socket->GetLocalPosition() - part.socket_pivot;

  // if the local alignment vector is zero, return an error
  if (local_alignment_vector == sf::Vector2f(0.f, 0.f)) {
    return std::unexpected(FailInfo{
        FailMode::InvalidState,
        std::format("Socket ID {} has zero-length local alignment vector.",
                    socket_id)});
  }

  // using the total rotation of the joint, rotate the local alignment vector to
  // get the world alignment vector
  sf::Transform rotation_transform;
  rotation_transform.rotate(total_rotation);

  const sf::Vector2f return_vector =
      rotation_transform.transformPoint(local_alignment_vector);

  return return_vector.normalized();
}

/////////////////////////////////////////////////
void maximise_joint_socket_spread(JointInstance::Sockets &joint_sockets,
                                  const sf::Vector2f &pivot,
                                  const SocketConfig &config) {

  const float arc_min = config.rotation_arc_min;
  const float arc_max = config.rotation_arc_max;
  const float arc_range = arc_max - arc_min;

  // if socket count is 0, return early
  if (joint_sockets.empty() || config.socket_count == 0) {
    return;
  }

  // if socket count is 1, place it at the midpoint of the arc
  if (joint_sockets.size() == 1) {
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
    for (size_t i = 0; i < joint_sockets.size(); ++i) {
      const float angle_deg = arc_min + (i * angle_between_sockets);
      const float angle_rad = sf::degrees(angle_deg).asRadians();
      const sf::Vector2f new_local_position =
          pivot + sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} *
                      config.radius;
      joint_sockets.at(i).SetLocalPosition(new_local_position);
    }
  }
}

} // namespace steamrot
