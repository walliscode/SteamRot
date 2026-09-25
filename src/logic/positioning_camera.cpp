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
sf::View get_ui_view(const sf::RenderTexture &texture) {
  return texture.getDefaultView();
}

/////////////////////////////////////////////////
void apply_world_view(sf::RenderTexture &texture,
                      const CameraState &camera_state) {
  texture.setView(get_world_view(camera_state, texture));
}

/////////////////////////////////////////////////
void apply_ui_view(sf::RenderTexture &texture) {
  texture.setView(get_ui_view(texture));
}

/////////////////////////////////////////////////
sf::Vector2f map_to_world_coords(const CameraState &camera_state,
                                 sf::Vector2i screen_pos,
                                 const sf::RenderTexture &texture) {
  return texture.mapPixelToCoords(screen_pos,
                                  get_world_view(camera_state, texture));
}

/////////////////////////////////////////////////
void update_world_mouse_position(sf::Vector2f &world_mouse_position,
                                 const CameraState &camera_state,
                                 sf::Vector2i screen_pos,
                                 const sf::RenderTexture &texture) {
  world_mouse_position = map_to_world_coords(camera_state, screen_pos, texture);
}

/////////////////////////////////////////////////
void apply_pan(CameraState &camera_state) {
  if (camera_state.m_panning_left)
    camera_state.m_position.x -= CameraState::kPanSpeed;
  if (camera_state.m_panning_right)
    camera_state.m_position.x += CameraState::kPanSpeed;
  if (camera_state.m_panning_up)
    camera_state.m_position.y -= CameraState::kPanSpeed;
  if (camera_state.m_panning_down)
    camera_state.m_position.y += CameraState::kPanSpeed;
}

} // namespace steamrot::logic::positioning::camera
