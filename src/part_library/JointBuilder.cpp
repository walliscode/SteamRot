/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the JointBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointBuilder.h"

namespace steamrot::parts {
/////////////////////////////////////////////////
JointBuilder &JointBuilder::AddViews(const PremadeViews view) {
  switch (view) {
  case PremadeViews::WhiteOblong:
    m_joint.positioning_views = get_white_oblong_Views();
    break;
  case PremadeViews::WhiteSquare:
    m_joint.positioning_views = get_white_square_Views();
    break;
  default:
    // Handle unknown view type if necessary
    break;
  }
  return *this;
}
/////////////////////////////////////////////////
JointBuilder &JointBuilder::SetOrigin(const sf::Vector2f &origin) {
  m_joint.socket_pivot = origin;
  return *this;
}
/////////////////////////////////////////////////
JointBuilder &JointBuilder::SetSocketConfig(
    const uint8_t socket_count, const float radius, const float min_gap,
    const float fixed_socket_angle, const bool has_fixed_socket,
    const float rotation_arc_min, const float rotation_arc_max) {
  m_joint.socket_config =
      SocketConfig(socket_count, radius, min_gap, fixed_socket_angle,
                   has_fixed_socket, rotation_arc_min, rotation_arc_max);
  return *this;
}
/////////////////////////////////////////////////
Joint JointBuilder::Build(const std::string &name) {
  m_joint.name = name;
  Joint joint_copy = m_joint;
  m_joint = Joint(); // Reset to a new Joint for future use
  // Return the constructed Joint object
  return joint_copy;
}
} // namespace steamrot::parts
