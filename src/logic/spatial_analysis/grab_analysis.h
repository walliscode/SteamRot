/////////////////////////////////////////////////
/// @file
/// @brief [TODO:description]
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for the spatial analysis of a grab
/// structure
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DescriptorResult.h"
#include "MachinaFormScaffold.h"
#include <SFML/System/Vector2.hpp>
#include <set>

namespace steamrot::logic::spatial_analysis {

/////////////////////////////////////////////////
/// @brief Returns a reference to the anchor joint instance for a grab
/// structure.
///
/// @param grab_result The GrabResult containing the anchor point ID for the
/// grab structure
/// @param part_graph The PartGraph containing the parts for the grab structure
/// @return A reference to the JointInstance corresponding to the anchor point
/// of the grab structure
/////////////////////////////////////////////////
JointInstance &get_anchor_joint(const GrabResult &grab_result,
                                PartGraph &part_graph);

std::set<uint32_t> get_connected_sockets(const PartInstance &anchor_joint);
/////////////////////////////////////////////////
/// @brief Get the number of connected sockets on the anchor joint and assign
/// the socket IDs to the left and right arm sockets
///
/// @param anchor_joint The JointInstance corresponding to the anchor point of
/// the grab structure
/// @param left_arm_sockets  A reference to a vector to be filled with the
/// socket IDs for the left arm of the grab structure
/// @param right_arm_sockets A reference to a vector to be filled with the
/// socket IDs for the right arm of the grab structure
/////////////////////////////////////////////////
void assign_left_and_right_arm_sockets(
    JointInstance &anchor_joint, std::vector<uint32_t> &left_arm_sockets,
    std::vector<uint32_t> &right_arm_sockets);

/////////////////////////////////////////////////
/// @brief Align the grab structure for easier analysis.
///
/// The anchor point is aligned to an/the origin and rotated until the y azis
/// bisects its rotation arc.
/////////////////////////////////////////////////
void align_grab_structure(const GrabResult &grab_result, PartGraph &part_graph,
                          const sf::Vector2f &anchor_point);

/////////////////////////////////////////////////
/// @brief Simple function to get the end of arm for a grab structure.
///
/// For now this just returns that last id in the Subgraph
/// @param arm Subgraph representing an arm of a grab structure
/// @return the part ID of the end of arm for the given arm
/////////////////////////////////////////////////
uint32_t get_end_of_arm(const SubGraph &arm);

} // namespace steamrot::logic::spatial_analysis
