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
uint32_t get_end_of_arm(const SubGraph &arm) {
  return arm.empty() ? 0 : arm.back();
}

/////////////////////////////////////////////////
void align_grab_result_to_open_state(GrabResult &grab_result,
                                     PartGraph &part_graph,
                                     const sf::Vector2f &anchor_point) {
  std::cout << "[align] Begin align_grab_result_to_open_state | anchor="
            << grab_result.anchor << " | arms=" << grab_result.arms.size()
            << " | anchor_point=(" << anchor_point.x << ", " << anchor_point.y
            << ")\n";

  // put out the anchor joint
  const uint32_t anchor_id = grab_result.anchor;

  // check that the anchor point is a valid part in the part graph
  if (!part_graph.contains(anchor_id)) {
    std::cerr << "[align][error] Anchor id " << anchor_id
              << " not found in part_graph. Aborting.\n";
    return;
  }

  JointInstance &anchor_joint = get_anchor_joint(grab_result, part_graph);
  std::cout << "[align] Resolved anchor joint id=" << anchor_id << "\n";

  // as this is the coordination fuction we can set the initial transform of the
  // anchor joint to identity
  anchor_joint.transform = sf::Transform::Identity;
  std::cout << "[align] Anchor transform reset to Identity\n";

  // LOCAL TRANSFORMS //
  // spread the sockets on the anchor joint to maximum
  positioning::grimoire_machina::maximise_joint_socket_spread(anchor_joint);
  std::cout << "[align] Anchor joint socket spread maximised\n";

  // GLOBAL TRANSFORMS //
  // align the anchor joint to the anchor point
  align_anchor_joint_to_anchor_point(anchor_joint, anchor_point);
  std::cout << "[align] Anchor joint aligned to anchor point\n";

  // create lambda to cycle through an arm an connect each one in turn to the
  // previous one
  auto align_arm = [&](const SubGraph &arm) {
    std::cout << "[align][arm] Start arm | size=" << arm.size() << "\n";

    // cycle through the subgraph - this should be in order of connection
    for (size_t i = 0; i < arm.size(); ++i) {
      std::cout << "[align][arm] aligning first part of the arm"
                << "\n";
      const uint32_t part_id = arm[i];
      std::cout << "[align][arm] i=" << i << " part_id=" << part_id << "\n";

      if (!part_graph.contains(part_id)) {
        std::cout << "[align][arm][skip] part_id=" << part_id
                  << " not in part_graph\n";
        continue;
      }

      // for the first part in the arm we need to check it connects to the
      // anchor joint
      if (i == 0) {
        std::cout << "[align][arm] First part, checking connection to anchor\n";

        // check that the part is connected to the anchor joint
        auto connection = action::grimoire_machina::check_for_connected_sockets(
            anchor_joint, std::get<FragmentInstance>(part_graph.at(part_id)));

        if (!connection) {
          std::cout << "[align][arm][skip] part_id=" << part_id
                    << " not connected to anchor\n";
          continue;
        }

        // get the part and check it is a FragmentInstance
        if (!std::holds_alternative<FragmentInstance>(part_graph.at(part_id))) {
          std::cout << "[align][arm][skip] part_id=" << part_id
                    << " is not FragmentInstance\n";
          continue;
        }

        FragmentInstance &fi =
            std::get<FragmentInstance>(part_graph.at(part_id));

        std::cout << "[align][arm] Align fragment part_id=" << part_id
                  << " onto anchor | fragment_socket="
                  << connection->fragment_socket_id
                  << " joint_socket=" << connection->joint_socket_id << "\n";

        // align the part to the anchor joint
        positioning::grimoire_machina::align_fragment_onto_joint_socket(
            fi, connection->fragment_socket_id, anchor_joint,
            connection->joint_socket_id);
      }

      // then cycle through the rest of the arm and align each part to the
      // previous one
      else {
        const uint32_t prev_part_id = arm[i - 1];
        std::cout << "[align][arm] Checking against prev_part_id="
                  << prev_part_id << "\n";

        if (!part_graph.contains(prev_part_id)) {
          std::cout << "[align][arm][skip] prev_part_id=" << prev_part_id
                    << " not in part_graph\n";
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

          std::cout << "[align][arm] Case Fragment <- Joint | part_id="
                    << part_id << " prev_part_id=" << prev_part_id << "\n";

          // check that the part is connected to the previous part
          auto connection =
              action::grimoire_machina::check_for_connected_sockets(ji, fi);
          if (!connection) {
            std::cout << "[align][arm][skip] No connection Fragment(part_id="
                      << part_id << ") <-> Joint(prev_part_id=" << prev_part_id
                      << ")\n";
            continue;
          }

          std::cout
              << "[align][arm] Align fragment onto joint | fragment_socket="
              << connection->fragment_socket_id
              << " joint_socket=" << connection->joint_socket_id << "\n";

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

          std::cout << "[align][arm] Case Joint <- Fragment | part_id="
                    << part_id << " prev_part_id=" << prev_part_id << "\n";

          // check that the part is connected to the previous part
          auto connection =
              action::grimoire_machina::check_for_connected_sockets(ji, fi);
          if (!connection) {
            std::cout << "[align][arm][skip] No connection Joint(part_id="
                      << part_id
                      << ") <-> Fragment(prev_part_id=" << prev_part_id
                      << ")\n";
            continue;
          }

          // maximise the socket spread on the joint before aligning it to the
          // fragment
          positioning::grimoire_machina::maximise_joint_socket_spread(ji);
          std::cout << "[align][arm] Joint socket spread maximised for part_id="
                    << part_id << "\n";

          std::cout << "[align][arm] Align joint onto fragment | joint_socket="
                    << connection->joint_socket_id
                    << " fragment_socket=" << connection->fragment_socket_id
                    << "\n";

          // align the part to the previous part
          positioning::grimoire_machina::align_joint_onto_fragment_socket(
              ji, connection->joint_socket_id, fi,
              connection->fragment_socket_id);
        } else {
          std::cout
              << "[align][arm][skip] Unsupported type pairing for part_id="
              << part_id << " and prev_part_id=" << prev_part_id << "\n";
        }
      }
    }

    std::cout << "[align][arm] End arm\n";
  };

  // cycle through the arms and align them
  for (size_t arm_index = 0; arm_index < grab_result.arms.size(); ++arm_index) {
    std::cout << "[align] Processing arm " << arm_index << "\n";
    align_arm(grab_result.arms[arm_index]);
  }

  std::cout << "[align] End align_grab_result_to_open_state\n";
}

} // namespace steamrot::logic::spatial_analysis
