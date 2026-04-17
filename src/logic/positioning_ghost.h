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
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::positioning::ghost {

/////////////////////////////////////////////////
/// @brief Update the world-space position of MrGhost to match the current
/// mouse cursor, and cache the result in world_mouse_position.
///
/// Converts the screen-space mouse position to world-space coordinates using
/// movement::camera::MapToWorldCoords (the single authoritative conversion), writes
/// the result into both @p world_mouse_position and @p mr_ghost.m_position,
/// and should be called once per tick before any world-space system consumes
/// either value.
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

} // namespace steamrot::logic::positioning::ghost
