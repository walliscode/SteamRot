/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for collision handling of Grimoire
/// Machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaFormScaffold.h"

namespace steamrot::logic::collision::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Checks whether mouse is over the growth point and changes bool
/// accordingly.
///
/// @param mouse_position Mouse position in world coordinates.
/// @param growth_point Growth point to check against.
/////////////////////////////////////////////////
void CheckMouseOverGrowthPoint(const sf::Vector2i &mouse_position,
                               GrowthPoint &growth_point);
} // namespace steamrot::logic::collision::grimoire_machina
