/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for positioning MrGhost.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MrGhost.h"
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::positioning::ghost {

/////////////////////////////////////////////////
/// @brief Update the position of MrGhost to match the current mouse position.
///
/// Converts the integer mouse position to a floating-point vector and writes
/// it into mr_ghost.m_position so that other Logic classes can use it to
/// render or place the ghost item at the cursor each tick.
///
/// @param mr_ghost      MrGhost instance whose position will be updated.
/// @param mouse_position Current mouse position in window coordinates.
/////////////////////////////////////////////////
void UpdatePosition(MrGhost &mr_ghost, const sf::Vector2i &mouse_position);

} // namespace steamrot::logic::positioning::ghost
