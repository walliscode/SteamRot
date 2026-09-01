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

namespace steamrot::logic::spatial_analysis {

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
void align_anchor_joint_to_anchor_point(JointInstance &anchor_joint,
                                        const sf::Vector2f &anchor_point);

/////////////////////////////////////////////////
/// @brief Given a grab structure, align it to the open state.
///
/// This maximises any JointInstance socket positions and then aligns the
/// fragmnets along those sockets
///
/// @param grab_result GrabResult containing the grab structure to be aligned
/// @param part_graph PartGraph containing the parts for the grab structure
/// @param anchor_point The anchor point to align the grab structure to (usually
/// the origin)
/////////////////////////////////////////////////
void align_grab_result_to_open_state(GrabResult &grab_result,
                                     PartGraph &part_graph,
                                     const sf::Vector2f &anchor_point);

/////////////////////////////////////////////////
/// @brief Given a FragmentsInstance and a PartGraph, determine if the end of
/// arm is ready to grab.
///
/// This is determined by checking the alignment vector of the connected socket
/// and comparing it to the reference direction of the anchor joint. If the
/// alignment vector is at least colinear with the reference direction or
/// "pointing" inwards, then the end of arm is ready to grab.
///
/// @param arm Subgraph representing an arm of a grab structure
/// @param anchor_joint JointInstance corresponding to the anchor point of the
/// grab structure
/// @param part_graph PartGraph containing the parts for the grab structure
/// @return true if the end of arm is ready to grab, false otherwise
/////////////////////////////////////////////////
bool end_of_arm_is_grab_ready(const SubGraph &arm, const bool is_left_arm,
                              const JointInstance &anchor_joint,
                              const PartGraph &part_graph);

/////////////////////////////////////////////////
/// @brief Given a GrabResult and a PartGraph, determine if all arms are ready
/// to grab.
///
///
/// @param grab_result GrabResult containing the grab structure to be analyzed
/// @param part_graph PartGraph containing the parts for the grab structure
/// @return true if all arms are ready to grab, false otherwise
/////////////////////////////////////////////////
bool all_arms_are_grab_ready(const GrabResult &grab_result,
                             const PartGraph &part_graph);

/////////////////////////////////////////////////
/// @brief This identifies the JointInstances in
///
/// @param grab_result [TODO:parameter]
/// @param part_graph [TODO:parameter]
/////////////////////////////////////////////////
void cycle_though_potential_grab_states(GrabResult &grab_result,
                                        PartGraph &part_graph);

} // namespace steamrot::logic::spatial_analysis
