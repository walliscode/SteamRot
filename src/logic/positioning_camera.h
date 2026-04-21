/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for camera positioning and view
/// computation.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraState.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::positioning::camera {

/////////////////////////////////////////////////
/// @brief Build a world-space view for the given render texture.
///
/// Returns a view centred on @p camera_state.m_position with the current zoom
/// level applied. The world origin (0, 0) is visible at the screen centre when
/// m_position is the default {0, 0}.
///
/// @param camera_state Camera state providing position and zoom level.
/// @param texture      Render texture whose size is used to build the view.
/// @return sf::View representing the world-space camera view.
/////////////////////////////////////////////////
sf::View get_world_view(const CameraState &camera_state,
                        const sf::RenderTexture &texture);

/////////////////////////////////////////////////
/// @brief Convert a screen-space pixel position to world-space coordinates.
///
/// Applies the current world view to map the given screen-pixel position to
/// its corresponding world-space floating-point position. This is the single
/// authoritative screen-to-world conversion.
///
/// @param camera_state Camera state used to build the world view.
/// @param screen_pos   Screen-space pixel position (e.g. mouse cursor).
/// @param texture      Render texture required for the coordinate mapping.
/// @return World-space position corresponding to @p screen_pos.
/////////////////////////////////////////////////
sf::Vector2f map_to_world_coords(const CameraState &camera_state,
                                 sf::Vector2i screen_pos,
                                 const sf::RenderTexture &texture);

/////////////////////////////////////////////////
/// @brief Apply the current pan state to the camera position.
///
/// Each tick that a pan direction flag is active in @p camera_state, the
/// camera's world-space position is moved by CameraState::kPanSpeed units in
/// that direction.  Multiple flags may be active simultaneously (e.g. W+D
/// for diagonal movement).
///
/// @param camera_state Camera state providing pan flags and position to mutate.
/////////////////////////////////////////////////
void apply_pan(CameraState &camera_state);

} // namespace steamrot::logic::positioning::camera
