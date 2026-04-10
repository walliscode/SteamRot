/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GhostRenderLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostRenderLogic.h"
#include "render_ghost.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
GhostRenderLogic::GhostRenderLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GhostRenderLogic::ProcessLogic() {

  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  render::ghost::DrawGhostItem(m_scene_context.scene_texture,
                                m_scene_context.mr_ghost, grimoire_machina);
}

} // namespace steamrot::logic
