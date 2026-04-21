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
#include "SceneType.h"
#include "containers.h"
#include "entity_memory.h"
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
/// @brief Compute the width of the leftmost UI panel strip from the entity
///        pool.
///
/// Iterates every active CUserInterface component and returns the maximum
/// root-element width among those whose root element is anchored at
/// position.x == 0.  Returns 0 when no such panel exists (e.g. scenes with no
/// left-side toolbar).
///
/// @param pool EntityMemoryPool whose CUserInterface components are inspected.
/// @return Width in pixels of the widest left-anchored UI panel, or 0.
/////////////////////////////////////////////////
float get_left_ui_toolbar_width(const EntityMemoryPool &pool);

/////////////////////////////////////////////////
/// @brief Compute the initial world-space camera centre for a given scene.
///
/// Each scene may expose its world origin at a different screen position.
/// Calling this function places world {0, 0} at the visual centre of the
/// usable area for that scene type.
///
/// - Most scenes return {0, 0} so the world origin is centred on the full
///   render texture.
/// - SceneType::CRAFTING returns the centre of the area to the right of the
///   left-side UI toolbar, whose width is computed dynamically from @p pool.
///
/// @param scene_type   The type of the scene being initialised.
/// @param texture_size Size of the scene render texture in pixels.
/// @param pool         Entity pool from which the toolbar width is derived.
/// @return World-space position that should be assigned to
///         CameraState::m_position on scene creation.
/////////////////////////////////////////////////
sf::Vector2f get_scene_world_origin(SceneType scene_type,
                                    sf::Vector2u texture_size,
                                    const EntityMemoryPool &pool);

} // namespace steamrot::logic::positioning::camera
