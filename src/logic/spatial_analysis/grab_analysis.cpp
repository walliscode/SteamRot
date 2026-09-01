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
#include "SocketState.h"
#include "positioning_grimoire_machina.h"
#include <cmath>

namespace steamrot::logic::spatial_analysis {

/////////////////////////////////////////////////
void assign_left_and_right_arm_sockets(
    JointInstance &anchor_joint, std::vector<uint32_t> &left_arm_sockets,
    std::vector<uint32_t> &right_arm_sockets) {

  // get the connected socket IDs on the anchor joint
  std::set<uint32_t> connected_sockets = anchor_joint.GetConnectedSocketIds();

  // assign the first half of the connected socket IDs to the left arm and the
  // second half to the right arm (including the middle socket if there is an
  // odd number of connected sockets)

  size_t i = 0;
  for (const auto &socket_id : connected_sockets) {
    if (i < connected_sockets.size() / 2) {
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
  const float arc_min = anchor_joint.GetPart().socket_config.rotation_arc_min;
  const float arc_max = anchor_joint.GetPart().socket_config.rotation_arc_max;
  const float arc_mid = (arc_min + arc_max) / 2.f;

  // then we convert that to a sf::Angle object
  sf::Angle rotation_angle = sf::degrees(arc_mid);

  // TRANSFORM //
  anchor_joint.setRotation(rotation_angle);
  anchor_joint.setPosition(anchor_point);
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

  // check that the anchor point is a JointInstance
  if (!std::holds_alternative<JointInstance>(part_graph.at(anchor_id))) {
    return;
  }

  // pull out the anchor joint from the part graph
  JointInstance &anchor_joint =
      std::get<JointInstance>(part_graph.at(anchor_id));

  // for the open state, every joint instance should have its sockets positioned
  // to maximize the distance between them
  for (auto &[part_id, part_instance] : part_graph) {
    if (std::holds_alternative<JointInstance>(part_instance)) {
      JointInstance &ji = std::get<JointInstance>(part_instance);
      ji.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
    }
  }

  // align the anchor joint to the anchor point
  align_anchor_joint_to_anchor_point(anchor_joint, anchor_point);

  // the anchor joint is now aligned, and the sockets are positioned to maximize
  // distance, so all we need to do is a call a generic positioning function to
  // align the rest of the grab structure to the anchor joint. This is just for
  // the open state, so we don't need to worry about any other constraints or
  // collisions.

  std::unordered_set<uint32_t> visited;
  std::unordered_set<uint32_t> in_stack;
  auto positioning_result = positioning::grimoire_machina::position_from_node(
      part_graph, anchor_id, visited, in_stack);
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
      (anchor_joint.GetPart().socket_config.rotation_arc_min +
       anchor_joint.GetPart().socket_config.rotation_arc_max) /
      2.f;

  // add the anchor joint's total rotation to the arc mid degrees to get the
  // world rotation
  const sf::Angle world_arc_mid_degrees =
      anchor_joint.getRotation() + sf::degrees(arc_mid_degrees);

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
  for (const auto &[socket_id, socket_data] : end_of_arm_fi.GetSockets()) {
    if (socket_data.GetConnectionState() == SocketConnectionState::Connected) {
      // get the alignment vector of the connected socket in world space
      // we are not built to deal with std::expected yet, so we will just use a
      // default vector if the socket does not exist
      alignment_vector =
          end_of_arm_fi.GetSocketWorldAlignmentVector(socket_id).value_or(
              sf::Vector2f(1.f, 0.f));
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

/////////////////////////////////////////////////
bool all_arms_are_grab_ready(const GrabResult &grab_result,
                             const PartGraph &part_graph) {
  // if the grab result has no arms, return false
  if (grab_result.arms.empty()) {
    return false;
  }

  // get the anchor joint from the part graph
  if (!part_graph.contains(grab_result.anchor)) {
    return false;
  }

  if (!std::holds_alternative<JointInstance>(
          part_graph.at(grab_result.anchor))) {
    return false;
  }
  const JointInstance &anchor_joint =
      std::get<JointInstance>(part_graph.at(grab_result.anchor));

  // check each arm for grab readiness
  for (size_t i = 0; i < grab_result.arms.size(); ++i) {
    const SubGraph &arm = grab_result.arms[i];
    const bool is_left_arm =
        i % 2 == 1; // odd index is left arm, even index is right arm
    if (!end_of_arm_is_grab_ready(arm, is_left_arm, anchor_joint, part_graph)) {
      return false;
    }
  }
  return true;
}
} // namespace steamrot::logic::spatial_analysis
