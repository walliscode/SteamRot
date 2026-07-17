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
#include <iostream>

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

  // update state of the anchor joint
  anchor_joint.total_rotation += rotation_angle;
};

/////////////////////////////////////////////////
void align_grab_result_to_open_state(GrabResult &grab_result,
                                     PartGraph &part_graph,
                                     const sf::Vector2f &anchor_point) {

  // put out the anchor joint
  const uint32_t anchor_id = grab_result.anchor;

  // check that the anchor point is a valid part in the part graph
  if (!part_graph.contains(anchor_id)) {
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

      if (!part_graph.contains(part_id)) {
        continue;
      }

      // for the first part in the arm we need to check it connects to the
      // anchor joint
      if (i == 0) {

        // check that the part is connected to the anchor joint
        auto connection = action::grimoire_machina::check_for_connected_sockets(
            anchor_joint, std::get<FragmentInstance>(part_graph.at(part_id)));

        if (!connection) {
          continue;
        }

        // get the part and check it is a FragmentInstance
        if (!std::holds_alternative<FragmentInstance>(part_graph.at(part_id))) {
          continue;
        }

        FragmentInstance &fi =
            std::get<FragmentInstance>(part_graph.at(part_id));

        // align the part to the anchor joint
        positioning::grimoire_machina::align_fragment_onto_joint_socket(
            fi, connection->fragment_socket_id, anchor_joint,
            connection->joint_socket_id);
      }

      // then cycle through the rest of the arm and align each part to the
      // previous one
      else {
        const uint32_t prev_part_id = arm[i - 1];

        if (!part_graph.contains(prev_part_id)) {
          continue;
        }

        // check the current part type
        if (std::holds_alternative<FragmentInstance>(part_graph.at(part_id)) &&
            std::holds_alternative<JointInstance>(
                part_graph.at(prev_part_id))) {
          FragmentInstance &fi =
              std::get<FragmentInstance>(part_graph.at(part_id));
          JointInstance &ji =
              std::get<JointInstance>(part_graph.at(prev_part_id));

          // check that the part is connected to the previous part
          auto connection =
              action::grimoire_machina::check_for_connected_sockets(ji, fi);
          if (!connection) {

            continue;
          }

          // align the part to the previous part
          positioning::grimoire_machina::align_fragment_onto_joint_socket(
              fi, connection->fragment_socket_id, ji,
              connection->joint_socket_id);

        } else if (std::holds_alternative<JointInstance>(
                       part_graph.at(part_id)) &&
                   std::holds_alternative<FragmentInstance>(
                       part_graph.at(prev_part_id))) {
          JointInstance &ji = std::get<JointInstance>(part_graph.at(part_id));
          FragmentInstance &fi =
              std::get<FragmentInstance>(part_graph.at(prev_part_id));

          // check that the part is connected to the previous part
          auto connection =
              action::grimoire_machina::check_for_connected_sockets(ji, fi);
          if (!connection) {
          }

          // maximise the socket spread on the joint before aligning it to the
          // fragment
          positioning::grimoire_machina::maximise_joint_socket_spread(ji);

          // align the part to the previous part
          positioning::grimoire_machina::align_joint_onto_fragment_socket(
              ji, connection->joint_socket_id, fi,
              connection->fragment_socket_id);
        } else {
        }
      }
    }
  };

  // cycle through the arms and align them
  for (size_t arm_index = 0; arm_index < grab_result.arms.size(); ++arm_index) {

    align_arm(grab_result.arms[arm_index]);
  }
}

/////////////////////////////////////////////////
bool end_of_arm_is_grab_ready(const SubGraph &arm, const bool is_left_arm,
                              const JointInstance &anchor_joint,
                              const PartGraph &part_graph) {

  // if the arm is empty or the part graph is empty, return false
  if (arm.empty() || part_graph.empty()) {
    return false;
  }

  // the midpoint of the anchor joint's rotation arc in world space gives us our
  // reference direction
  const float arc_mid_degrees =
      (anchor_joint.joint->socket_config.rotation_arc_min +
       anchor_joint.joint->socket_config.rotation_arc_max) /
      2.f;

  // add the anchor joint's total rotation to the arc mid degrees to get the
  // world rotation
  const sf::Angle world_arc_mid_degrees =
      anchor_joint.total_rotation + sf::degrees(arc_mid_degrees);

  // convert the world arc mid degrees to a sf::Vector2f representing the
  // direction. Makes use of the normalized vector to get the direction only
  sf::Vector2f reference_direction =
      sf::Vector2f(std::cos(world_arc_mid_degrees.asRadians()),
                   std::sin(world_arc_mid_degrees.asRadians()))
          .normalized();

  // get the end of arm part ID and check that it is a FragmentInstance
  const uint32_t end_of_arm_id = arm.back();
  if (!part_graph.contains(end_of_arm_id) ||
      !std::holds_alternative<FragmentInstance>(part_graph.at(end_of_arm_id))) {
    return false;
  }

  // get the end of arm FragmentInstance
  const FragmentInstance &end_of_arm_fi =
      std::get<FragmentInstance>(part_graph.at(end_of_arm_id));

  // find the alignment vector of the connected socket
  sf::Vector2f alignment_vector;
  for (const auto &[socket_id, socket_data] : end_of_arm_fi.sockets) {
    if (socket_data.connection_state == SocketConnectionState::Connected) {
      alignment_vector =
          positioning::grimoire_machina::calculate_alignment_vector(
              end_of_arm_fi, socket_id);
      break;
    }
  }

  // find the cross product to get directionality.
  const float direction = reference_direction.cross(alignment_vector);

  // if the arm is the left arm, the direction should be equal to or negative
  // (counter clockwise) to the reference direction. If the arm is the right
  // arm, the direction should be equal to or positive (clockwise) to the
  // reference direction.
  //
  // add a small epsilon to account for floating point errors
  constexpr float EPS = 1e-5f;

  // left arm: allow negative or near-zero
  if (is_left_arm && direction > EPS) {
    return false;
  }
  // right arm: allow positive or near-zero
  if (!is_left_arm && direction < -EPS) {
    return false;
  }
  return true;
}

} // namespace steamrot::logic::spatial_analysis
