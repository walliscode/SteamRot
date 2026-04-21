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

/////////////////////////////////////////////////
sf::Vector2f get_scene_world_origin(SceneType scene_type,
                                    sf::Vector2u texture_size) {
  switch (scene_type) {
  case SceneType::CRAFTING: {
    const float usable_width =
        static_cast<float>(texture_size.x) - kCraftingUIToolbarWidth;
    return {kCraftingUIToolbarWidth + usable_width / 2.f,
            static_cast<float>(texture_size.y) / 2.f};
  }
  default:
    return {0.f, 0.f};
  }
}

} // namespace steamrot::logic::positioning::camera
