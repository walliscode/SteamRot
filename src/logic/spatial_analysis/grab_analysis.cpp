/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for the spatial analysis of a grab
/// structure
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "grab_analysis.h"
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::spatial_analysis {

/////////////////////////////////////////////////
void align_grab_structure(const GrabResult &grab_result, PartGraph &part_graph,
                          const sf::Vector2f &anchor_point) {

  // pull out the variables
  const uint32_t anchor_id = grab_result.anchor;

  // check that the anchor point is a valid part in the part graph
  if (!part_graph.contains(anchor_id)) {

    // [TODO:] potenially add some kind analysis event here to log this failure
    return;
  }

  // check that the anchor point is a joint instance
  if (!std::holds_alternative<JointInstance>(part_graph.at(anchor_id)))
    return;

  JointInstance &anchor_joint =
      std::get<JointInstance>(part_graph.at(anchor_id));

  // reset the anchor joint's transformation and set the transformation to the
  // anchor point
  anchor_joint.transform = sf::Transform::Identity;
  anchor_joint.transform.translate(anchor_point);
};

/////////////////////////////////////////////////
uint32_t get_end_of_arm(const SubGraph &arm) {
  return arm.empty() ? 0 : arm.back();
}

} // namespace steamrot::logic::spatial_analysis
