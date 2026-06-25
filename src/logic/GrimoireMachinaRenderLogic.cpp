/////////////////////////////////////////////////
/// @file
/// @brief Implemetation of the GrimoireMachinaRenderLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaRenderLogic.h"
#include "positioning_camera.h"
#include "render_grimoire_machina.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaRenderLogic::GrimoireMachinaRenderLogic(
    const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GrimoireMachinaRenderLogic::ProcessLogic() {

  // Apply the world (zoomed) view before drawing world-space geometry.
  m_scene_context.scene_texture.setView(positioning::camera::get_world_view(
      m_scene_context.camera_state, m_scene_context.scene_texture));

  // --- Asset section ---
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  // pull out the default font for use
  auto font_result = m_scene_context.asset_manager.GetFont("Roboto-Regular");
  if (!font_result.has_value()) {
    return;
  }
  const sf::Font &font = *font_result.value();
  render::grimoire_machina::render_machina_form(m_scene_context.scene_texture,
                                                grimoire_machina, font);
}

} // namespace steamrot::logic
