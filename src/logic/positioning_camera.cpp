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
float get_left_ui_toolbar_width(const EntityMemoryPool &pool) {
  const size_t pool_size = entity::memory::GetMemoryPoolSize(pool);
  float max_width = 0.f;
  for (size_t i = 0; i < pool_size; ++i) {
    const CUserInterface &ui =
        entity::memory::GetComponent<CUserInterface>(i, pool);
    if (!ui.m_active || !ui.m_root_element)
      continue;
    if (ui.m_root_element->position.x == 0.f &&
        ui.m_root_element->size.x > 0.f) {
      max_width = std::max(max_width, ui.m_root_element->size.x);
    }
  }
  return max_width;
}

/////////////////////////////////////////////////
sf::Vector2f get_scene_world_origin(SceneType scene_type,
                                    sf::Vector2u texture_size,
                                    const EntityMemoryPool &pool) {
  switch (scene_type) {
  case SceneType::CRAFTING: {
    const float toolbar_width = get_left_ui_toolbar_width(pool);
    const float usable_width =
        static_cast<float>(texture_size.x) - toolbar_width;
    return {toolbar_width + usable_width / 2.f,
            static_cast<float>(texture_size.y) / 2.f};
  }
  default:
    return {0.f, 0.f};
  }
}

} // namespace steamrot::logic::positioning::camera
