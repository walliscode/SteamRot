/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaFormScaffold.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Calculates the rotation angle required to rotate a source vector to
/// align with a target vector.
///
/// @param source_vector the source vector to be rotated
/// @param target_vector the target vector to align with
/////////////////////////////////////////////////
sf::Angle
rotation_of_vector_to_target_vector(const sf::Vector2f &source_vector,
                                    const sf::Vector2f &target_vector);

/////////////////////////////////////////////////
/// @brief Identifies the PartInstance with a socket index of 0 and positions at
/// 0,0 of the canvas
/////////////////////////////////////////////////
void position_first_part_of_machina_form_scaffold(PartGraph &parts);

/////////////////////////////////////////////////
/// @brief Wrapper function to completely position the parts of the
/// MachinaFormScaffold.
///
/// @param parts The PartGraph of the MachinaFormScaffold to position
/////////////////////////////////////////////////
void position_machina_form_scaffold(PartGraph &parts);

/////////////////////////////////////////////////
/// @brief Starting from PartInstance 0 of the PartGraph, positions all parts in
/// the graph based on their connections and socket configurations.
///
/// @param part_graph PartGraph to position
/////////////////////////////////////////////////
void position_part_graph(PartGraph &part_graph);

/////////////////////////////////////////////////
/// @brief expands teh composite box to include the next box, effectively
/// calculating the bounding box of all boxes added so far.
///
/// @param composite_box sf::FloatRect that will be expanded to include the next
/// box
/// @param next_box sf::FloatRect that will be used to expand the composite box
/////////////////////////////////////////////////
void calculate_composite_box(sf::FloatRect &composite_box,
                             const sf::FloatRect &next_box);

/////////////////////////////////////////////////
/// @brief Helper function to get the transformed bounding box of a
/// PartInstanceVariant.
///
/// @param part_variant PartInstanceVariant to get the transformed bounding box
/// of
/////////////////////////////////////////////////
sf::FloatRect
get_transformed_bounding_box(const PartInstanceVariant &part_variant);

/////////////////////////////////////////////////
/// @brief Given a PartGraph and an optional SubGraph, calculates the outer
/// bounding box of the parts
///
/// @param part_graph PartGraph containing the parts to calculate the outer
/// bounding box for
/// @param sub_graph SubGraph containing a subset of the parts to calculate the
/// outer bounding box for (optional)
/////////////////////////////////////////////////
sf::FloatRect calculate_outer_box(const PartGraph &part_graph,
                                  const SubGraph &sub_graph = {},
                                  const bool use_minimal_bounding_box = true);

} // namespace steamrot::logic::positioning::grimoire_machina
