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

  // set the origin of the joint to the pivot point defined in the joint's
  // socket config
  setOrigin(joint.socket_pivot);
}

/////////////////////////////////////////////////
JointInstance::JointInstance(const uint32_t id, const Joint &joint,
                             const std::string &alias)
    : PartInstance<JointTraits>(id, alias, joint) {
  // initialise the sockets based on the joint's socket config
  for (uint32_t i = 0; i < joint.socket_config.socket_count; ++i) {
    sockets.emplace(i, JointSocketState{});
  }

  // set the origin of the joint to the pivot point defined in the joint's
  // socket config
  setOrigin(joint.socket_pivot);
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

  // translate the socket and pivot to world space using the joint's transform
  const sf::Vector2f socket_world =
      getTransform().transformPoint(socket->GetLocalPosition());
  const sf::Vector2f pivot_world =
      getTransform().transformPoint(part.socket_pivot);

  // calculate the alignment vector from the pivot to the socket in world space
  const sf::Vector2f alignment_vector = socket_world - pivot_world;

  // return the normalized alignment vector
  return alignment_vector.normalized();
}

/////////////////////////////////////////////////
void maximise_joint_socket_spread(JointInstance::Sockets &joint_sockets,
                                  const sf::Vector2f &pivot,
                                  const SocketConfig &config) {
  // Nothing to place.
  if (joint_sockets.empty()) {
    return;
  }

  const float arc_min_deg = config.rotation_arc_min;
  const float arc_max_deg = config.rotation_arc_max;
  const float arc_range_deg = arc_max_deg - arc_min_deg; // can be negative
  const size_t count = joint_sockets.size();

  // Helper: normalize any degree angle into [0, 360).
  auto normalize_deg_0_360 = [](float deg) -> float {
    float n = std::fmod(deg, 360.f);
    if (n < 0.f)
      n += 360.f;
    return n;
  };

  // Helper: place one socket at a given degree angle.
  auto place_socket_at_deg = [&](size_t index, float angle_deg) {
    const float normalized_deg = normalize_deg_0_360(angle_deg);
    const float angle_rad = sf::degrees(normalized_deg).asRadians();

    const sf::Vector2f dir{std::cos(angle_rad), std::sin(angle_rad)};
    const sf::Vector2f local_pos = pivot + dir * config.radius;
    joint_sockets[index].SetLocalPosition(local_pos);
  };

  // Single socket: place at arc midpoint (works for positive/negative ranges).
  if (count == 1) {
    const float midpoint_deg = arc_min_deg + 0.5f * arc_range_deg;
    place_socket_at_deg(0, midpoint_deg);
    return;
  }

  // For partial arcs (< 360° magnitude), include both endpoints:
  //   step = range / (count - 1)
  //
  // For full circles (>= 360° magnitude), do NOT include both endpoints,
  // because min and max represent the same direction modulo 360°.
  //   step = range / count
  //
  // This also behaves sensibly for |range| > 360: extra turns are distributed
  // continuously, but no duplicate "last == first" endpoint is forced.
  const bool is_full_circle_or_more = std::abs(arc_range_deg) >= 360.f;
  const float divisor = is_full_circle_or_more ? static_cast<float>(count)
                                               : static_cast<float>(count - 1);
  const float step_deg = arc_range_deg / divisor; // preserves sign (CW/CCW)

  for (size_t i = 0; i < count; ++i) {
    const float angle_deg = arc_min_deg + static_cast<float>(i) * step_deg;
    place_socket_at_deg(i, angle_deg);
  }
}

} // namespace steamrot
