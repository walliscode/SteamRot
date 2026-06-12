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
