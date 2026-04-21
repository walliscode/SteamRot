/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for camera positioning and view
/// computation.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_camera.h"
#include "CUserInterface.h"

namespace steamrot::logic::positioning::camera {

/////////////////////////////////////////////////
float get_left_ui_toolbar_width(const EntityMemoryPool &pool) {
  const size_t pool_size = entity::memory::GetMemoryPoolSize(pool);
  float max_width = 0.f;
  for (size_t i = 0; i < pool_size; ++i) {
    const CUserInterface &ui =
        entity::memory::GetComponent<CUserInterface>(i, pool);
    if (!ui.m_active || !ui.m_root_element ||
        ui.m_root_element->position.x != 0.f ||
        ui.m_root_element->size.x <= 0.f)
      continue;
    max_width = std::max(max_width, ui.m_root_element->size.x);
  }
  return max_width;
}

/////////////////////////////////////////////////
sf::Vector2f get_scene_view_offset(SceneType scene_type,
                                   const EntityMemoryPool &pool) {
  if (scene_type == SceneType::CRAFTING) {
    return {-get_left_ui_toolbar_width(pool) / 2.f, 0.f};
  }
  return {0.f, 0.f};
}

/////////////////////////////////////////////////
sf::View get_world_view(const CameraState &camera_state,
                        const sf::RenderTexture &texture,
                        SceneType scene_type,
                        const EntityMemoryPool &pool) {
  sf::View view = texture.getDefaultView();
  view.setCenter(camera_state.m_position +
                 get_scene_view_offset(scene_type, pool));
  view.zoom(camera_state.m_zoom_level);
  return view;
}

/////////////////////////////////////////////////
sf::Vector2f map_to_world_coords(const CameraState &camera_state,
                                 sf::Vector2i screen_pos,
                                 const sf::RenderTexture &texture,
                                 SceneType scene_type,
                                 const EntityMemoryPool &pool) {
  return texture.mapPixelToCoords(
      screen_pos, get_world_view(camera_state, texture, scene_type, pool));
}

} // namespace steamrot::logic::positioning::camera
