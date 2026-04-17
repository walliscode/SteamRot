/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning MrGhost.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_ghost.h"
#include "movement_camera.h"

namespace steamrot::logic::positioning::ghost {

/////////////////////////////////////////////////
void UpdatePosition(MrGhost &mr_ghost, sf::Vector2f &world_mouse_position,
                    const sf::Vector2i &mouse_position,
                    const CameraState &camera_state,
                    const sf::RenderTexture &scene_texture) {
  world_mouse_position =
      steamrot::logic::movement::camera::MapToWorldCoords(
          camera_state, mouse_position, scene_texture);
  mr_ghost.m_position = world_mouse_position;
}

} // namespace steamrot::logic::positioning::ghost
