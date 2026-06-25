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
#include "CameraState.h"
#include "MrGhost.h"
#include "Subscriber.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

namespace steamrot::logic::positioning::ghost {

/////////////////////////////////////////////////
/// @brief Update the world-space position of MrGhost to match the current
/// mouse cursor, and cache the result in world_mouse_position.
///
/// Converts the screen-space mouse position to world-space coordinates using
/// positioning::camera::MapToWorldCoords (the single authoritative conversion),
/// writes the result into both @p world_mouse_position and @p
/// mr_ghost.m_position, and should be called once per tick before any
/// world-space system consumes either value. The stored @p
/// mr_ghost.m_rotation_degrees is applied to the instance transform so the
/// ghost is rendered at the correct rotation.
///
/// @param mr_ghost            MrGhost instance whose position will be updated.
/// @param world_mouse_position Out-parameter receiving the computed world-space
///                             cursor position (stored in SceneResources and
///                             exposed via SceneContext::world_mouse_position).
/// @param mouse_position       Screen-space pixel position of the mouse cursor.
/// @param camera_state         Camera/view state used for the coordinate
///                             conversion.
/// @param scene_texture        Render texture required by MapToWorldCoords.
/////////////////////////////////////////////////
void UpdatePosition(MrGhost &mr_ghost, sf::Vector2f &world_mouse_position,
                    const sf::Vector2i &mouse_position,
                    const CameraState &camera_state,
                    const sf::RenderTexture &scene_texture);

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
