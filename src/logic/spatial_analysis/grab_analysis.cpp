/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for the spatial analysis of a grab
/// structure
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "grab_analysis.h"
#include "MachinaFormScaffold.h"
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::spatial_analysis {

/////////////////////////////////////////////////
JointInstance &get_anchor_joint(const GrabResult &grab_result,
                                PartGraph &part_graph) {
  return std::get<JointInstance>(part_graph.at(grab_result.anchor));
}

/////////////////////////////////////////////////
std::set<uint32_t> get_connected_sockets(const JointInstance &anchor_joint) {

  std::set<uint32_t> connected_sockets;
  for (const auto &[socket_id, socket_data] : anchor_joint.sockets) {
    if (socket_data.connection_state == SocketConnectionState::Connected) {
      connected_sockets.insert(socket_id);
    }
  }
  return connected_sockets;
}
/////////////////////////////////////////////////
void assign_left_and_right_arm_sockets(
    JointInstance &anchor_joint, std::vector<uint32_t> &left_arm_sockets,
    std::vector<uint32_t> &right_arm_sockets) {

  // get the connected socket IDs on the anchor joint
  std::set<uint32_t> connected_sockets = get_connected_sockets(anchor_joint);

  // count the number of connected sockets
  size_t num_connected_sockets = connected_sockets.size();

  // assign the first half of the connected socket IDs to the left arm and the
  // second half to the right arm (including the middle socket if there is an
  // odd number of connected sockets)

  size_t i = 0;
  for (const auto &socket_id : connected_sockets) {
    if (i < num_connected_sockets / 2) {
      left_arm_sockets.push_back(socket_id);
    } else {
      right_arm_sockets.push_back(socket_id);
    }
    i++;
  }
}

/////////////////////////////////////////////////
void align_anchor_joint_to_anchor_point(const GrabResult &grab_result,
                                        PartGraph &part_graph,
                                        const sf::Vector2f &anchor_point) {

  // pull out the variables
  const uint32_t anchor_id = grab_result.anchor;

  // check that the anchor point is a valid part in the part graph
  if (!part_graph.contains(anchor_id)) {

    // [TODO:] potenially add some kind analysis event here to log this failure
    return;
  }

  JointInstance &anchor_joint = get_anchor_joint(grab_result, part_graph);

  // reset the anchor joint's transformation and set the transformation to the
  // anchor point
  anchor_joint.transform = sf::Transform::Identity;
  anchor_joint.transform.translate(anchor_point);

  // now we find the mid point of the Joint arc
  const float arc_min = anchor_joint.joint->socket_config.rotation_arc_min;
  const float arc_max = anchor_joint.joint->socket_config.rotation_arc_max;
  const float arc_mid = (arc_min + arc_max) / 2.f;

  // the downwards y axis is 90 degrees, so we need to rotate the anchor joint
  // by 90 - arc_mid
  anchor_joint.transform.rotate(sf::degrees(90.f - arc_mid));
};

/////////////////////////////////////////////////
uint32_t get_end_of_arm(const SubGraph &arm) {
  return arm.empty() ? 0 : arm.back();
}

/////////////////////////////////////////////////
void align_grab_result_to_open_state(GrabResult &grab_result,
                                     PartGraph &part_graph,
                                     const sf::Vector2f &anchor_point) {}

} // namespace steamrot::logic::spatial_analysis
