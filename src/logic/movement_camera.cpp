/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for camera movement and view
/// computation.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "movement_camera.h"
#include <algorithm>

namespace steamrot::logic::movement::camera {

/////////////////////////////////////////////////
void ApplyZoom(CameraState &camera_state, float scroll_delta) {
  camera_state.m_zoom_level *= (1.0f - scroll_delta * CameraState::kZoomStep);
  camera_state.m_zoom_level = std::clamp(camera_state.m_zoom_level,
                                          CameraState::kMinZoom,
                                          CameraState::kMaxZoom);
}

/////////////////////////////////////////////////
sf::View GetWorldView(const CameraState &camera_state,
                      const sf::RenderTexture &texture) {
  sf::View view = texture.getDefaultView();
  view.setCenter(camera_state.m_position);
  view.zoom(camera_state.m_zoom_level);
  return view;
}

/////////////////////////////////////////////////
sf::Vector2f MapToWorldCoords(const CameraState &camera_state,
                               sf::Vector2i screen_pos,
                               const sf::RenderTexture &texture) {
  return texture.mapPixelToCoords(screen_pos,
                                  GetWorldView(camera_state, texture));
}

} // namespace steamrot::logic::movement::camera
