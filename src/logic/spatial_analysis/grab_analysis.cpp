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
#include "action_grimoire_machina.h"
#include "positioning_grimoire_machina.h"
#include <SFML/System/Angle.hpp>
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
void align_anchor_joint_to_anchor_point(JointInstance &anchor_joint,
                                        const sf::Vector2f &anchor_point) {

  // first we build up the translation and rotation values we need, then build
  // up the transform

  // ROTATION //
  // first we find the mid point of the Joint arc in local space
  const float arc_min = anchor_joint.joint->socket_config.rotation_arc_min;
  const float arc_max = anchor_joint.joint->socket_config.rotation_arc_max;
  const float arc_mid = (arc_min + arc_max) / 2.f;

  // then we convert that to a sf::Angle object
  sf::Angle rotation_angle = sf::degrees(arc_mid);

  // then to a transform we an apply to the socket pivot
  sf::Transform rotation_transform{sf::Transform::Identity};
  rotation_transform.rotate(rotation_angle);

  // TRANSLATION //
  // we need to translate the socket_pivot to the anchor point, AFTER it has
  // been rotated. This sidesteps the need to specify a pivot point
  const sf::Vector2f translation_vector =
      anchor_point -
      rotation_transform.transformPoint(anchor_joint.joint->socket_pivot);

  // TRANSFORM //
  anchor_joint.transform.translate(translation_vector);
  anchor_joint.transform.rotate(rotation_angle);
};

/////////////////////////////////////////////////
uint32_t get_end_of_arm(const SubGraph &arm) {
  return arm.empty() ? 0 : arm.back();
}

/////////////////////////////////////////////////
void align_grab_result_to_open_state(GrabResult &grab_result,
                                     PartGraph &part_graph,
                                     const sf::Vector2f &anchor_point) {

  // put out the anchor joint
  const uint32_t anchor_id = grab_result.anchor;
  // check that the anchor point is a valid part in the part graph
  if (!part_graph.contains(anchor_id)) {

    // [TODO:] potenially add some kind analysis event here to log this failure
    return;
  }

  JointInstance &anchor_joint = get_anchor_joint(grab_result, part_graph);

  // as this is the coordination fuction we can set the initial transform of the
  // anchor joint to identity
  anchor_joint.transform = sf::Transform::Identity;

  // LOCAL TRANSFORMS //
  // spread the sockets on the anchor joint to maximum
  positioning::grimoire_machina::maximise_joint_socket_spread(anchor_joint);

  // GLOBAL TRANSFORMS //
  // align the anchor joint to the anchor point
  align_anchor_joint_to_anchor_point(anchor_joint, anchor_point);

  // create lambda to cycle through an arm an connect each one in turn to the
  // previous one
  auto align_arm = [&](const SubGraph &arm) {
    // cycle through the subgraph - this should be in order of connection
    for (size_t i = 0; i < arm.size(); ++i) {
      const uint32_t part_id = arm[i];

      // for the first part in the arm we need to check it connects to the
      // anchor joint
      if (i == 0) {

        // check that the part is connected to the anchor joint
        auto connection = action::grimoire_machina::check_for_connected_sockets(
            anchor_joint, std::get<FragmentInstance>(part_graph.at(part_id)));

        if (!connection)
          // skip this part if it is not connected to the anchor joint
          continue;

        // get the part and check it is a FragmentInstance
        if (!std::holds_alternative<FragmentInstance>(part_graph.at(part_id))) {
          // skip this part if it is not a FragmentInstance
          continue;
        }
        FragmentInstance &fi =
            std::get<FragmentInstance>(part_graph.at(part_id));

        // align the part to the anchor joint
        positioning::grimoire_machina::align_fragment_onto_joint_socket(
            fi, connection->fragment_socket_id, anchor_joint,
            connection->joint_socket_id);
      }
      if (!part_graph.contains(part_id)) {
        // skip this part if it is not in the part graph
        continue;
      }
    }
  };

  // cycle through the arms and align them
  for (const auto &arm : grab_result.arms) {
    align_arm(arm);
  }
}

} // namespace steamrot::logic::spatial_analysis
