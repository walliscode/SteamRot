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
/// @brief Update the world-space position and instance transform of MrGhost.
///
/// Consumes the already-computed scene-wide @p world_mouse_position, writes it
/// into mr_ghost.m_position, and rebuilds the selected ghost instance
/// transform using the stored mr_ghost.m_rotation_degrees. Screen-to-world
/// conversion is intentionally handled elsewhere by camera positioning code.
///
/// @param mr_ghost             MrGhost instance whose position will be updated.
/// @param world_mouse_position Shared world-space cursor position for the
///                             current tick.
/////////////////////////////////////////////////
void UpdatePosition(MrGhost &mr_ghost,
                    const sf::Vector2f &world_mouse_position);

/////////////////////////////////////////////////
/// @brief Rotate the ghost selection by 90 degrees.
///
/// Increments mr_ghost.m_rotation_degrees by 90 degrees (wrapping at 360).
/// The new rotation is applied the next time UpdatePosition is called.
///
/// @param mr_ghost MrGhost instance whose rotation will be incremented.
/////////////////////////////////////////////////
void RotateGhost(MrGhost &mr_ghost);

/////////////////////////////////////////////////
/// @brief Process all active ROTATE_GHOST subscribers and rotate the ghost.
///
/// Iterates @p subscribers; for each active subscriber whose
/// captured_payload holds an InputPayload with action ROTATE_GHOST,
/// calls RotateGhost once. This function is the authoritative place for
/// subscriber-driven rotation logic and is intended to be called from
/// GhostPositioningLogic::ProcessLogic().
///
/// @param subscribers Subscribers owned by the calling Logic instance.
/// @param mr_ghost    MrGhost instance whose rotation will be updated.
/////////////////////////////////////////////////
void ProcessRotateSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    MrGhost &mr_ghost);

} // namespace steamrot::logic::positioning::ghost
