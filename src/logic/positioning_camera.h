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
/// @brief Build a scene-aware world-space view for the given render texture.
///
/// The view centre is `camera_state.m_position + get_scene_view_offset(...)`.
/// This ensures world {0, 0} appears at the visual centre of the usable canvas
/// for the given scene type (e.g. to the right of a left-side toolbar in
/// CRAFTING) when camera_state.m_position is the default {0, 0}.
/// Camera panning is represented as a delta in camera_state.m_position.
///
/// @param camera_state Camera state providing pan offset and zoom level.
/// @param texture      Render texture whose size defines the viewport.
/// @param scene_type   Scene type used to select the per-scene offset.
/// @param pool         Entity pool from which the toolbar width is derived.
/// @return sf::View representing the world-space camera view.
/////////////////////////////////////////////////
sf::View get_world_view(const CameraState &camera_state,
                        const sf::RenderTexture &texture,
                        SceneType scene_type,
                        const EntityMemoryPool &pool);

/////////////////////////////////////////////////
/// @brief Convert a screen-space pixel position to world-space coordinates.
///
/// Applies the current world view (built with get_world_view) to map the
/// given screen-pixel position to its corresponding world-space position.
/// This is the single authoritative screen-to-world conversion.
///
/// @param camera_state Camera state used to build the world view.
/// @param screen_pos   Screen-space pixel position (e.g. mouse cursor).
/// @param texture      Render texture required for the coordinate mapping.
/// @param scene_type   Scene type forwarded to get_world_view.
/// @param pool         Entity pool forwarded to get_world_view.
/// @return World-space position corresponding to @p screen_pos.
/////////////////////////////////////////////////
sf::Vector2f map_to_world_coords(const CameraState &camera_state,
                                 sf::Vector2i screen_pos,
                                 const sf::RenderTexture &texture,
                                 SceneType scene_type,
                                 const EntityMemoryPool &pool);

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
/// @brief Compute the view-centre offset that places world {0, 0} at the
///        visual centre of the usable canvas for a given scene type.
///
/// The offset is added to camera_state.m_position inside get_world_view so
/// that the default (zero) pan shows the world origin at the right screen
/// position for each scene:
///
/// - SceneType::CRAFTING: `{ -toolbar_width / 2, 0 }` where toolbar_width is
///   read from @p pool via get_left_ui_toolbar_width.
/// - All other scenes: `{ 0, 0 }`.
///
/// @param scene_type Scene type selecting the per-scene offset formula.
/// @param pool       Entity pool from which the toolbar width is derived.
/// @return The offset vector to add to camera_state.m_position.
/////////////////////////////////////////////////
sf::Vector2f get_scene_view_offset(SceneType scene_type,
                                   const EntityMemoryPool &pool);

} // namespace steamrot::logic::positioning::camera
