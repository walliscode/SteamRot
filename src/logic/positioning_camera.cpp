/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for camera positioning and view
/// computation.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_camera.h"

namespace steamrot::logic::positioning::camera {

/////////////////////////////////////////////////
sf::View get_world_view(const CameraState &camera_state,
                        const sf::RenderTexture &texture) {
  sf::View view = texture.getDefaultView();
  view.setCenter(camera_state.m_position);
  view.zoom(camera_state.m_zoom_level);
  return view;
}

/////////////////////////////////////////////////
sf::Vector2f map_to_world_coords(const CameraState &camera_state,
                                 sf::Vector2i screen_pos,
                                 const sf::RenderTexture &texture) {
  return texture.mapPixelToCoords(screen_pos,
                                  get_world_view(camera_state, texture));
}

} // namespace steamrot::logic::positioning::camera
