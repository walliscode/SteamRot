/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GhostRenderLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostRenderLogic.h"
#include "movement_camera.h"
#include "render_ghost.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
GhostRenderLogic::GhostRenderLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GhostRenderLogic::ProcessLogic() {

  // Apply the world (zoomed) view so the ghost is positioned in world space.
  m_scene_context.scene_texture.setView(
      movement::camera::GetWorldView(m_scene_context.camera_state,
                                     m_scene_context.scene_texture));

  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  render::ghost::DrawGhostItem(m_scene_context.scene_texture,
                                m_scene_context.mr_ghost, grimoire_machina);
}

} // namespace steamrot::logic
