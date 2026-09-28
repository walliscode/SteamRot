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
#include "Subscriber.h"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

namespace steamrot::logic::positioning::ghost {

/////////////////////////////////////////////////
/// @brief Updates all positioning information for the ghost. translation,
/// rotation e.t.c.
///
/// @param mr_ghost [TODO:parameter]
/// @param world_mouse_position [TODO:parameter]
/////////////////////////////////////////////////
void update_position(MrGhost &mr_ghost,
                     const sf::Vector2f &world_mouse_position);

/////////////////////////////////////////////////
/// @brief Rotate the ghost selection by 90 degrees.
///
/// Increments mr_ghost.m_rotation_degrees by 90 degrees (wrapping at 360).
/// The new rotation is applied the next time UpdatePosition is called.
///
/// @param mr_ghost MrGhost instance whose rotation will be incremented.
/////////////////////////////////////////////////
void rotate_ghost(MrGhost &mr_ghost);

/////////////////////////////////////////////////
/// @brief Process all active subscribers and update MrGhost
///
/// @param subscribers Subscribers owned by the calling Logic instance.
/// @param mr_ghost    MrGhost instance whose rotation will be updated.
/////////////////////////////////////////////////
void process_subscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    MrGhost &mr_ghost);

} // namespace steamrot::logic::positioning::ghost
