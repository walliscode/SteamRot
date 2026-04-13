/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GrimoireMachinaCollisionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaCollisionLogic.h"
#include "collision_mouse.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
GrimoireMachinaCollisionLogic::GrimoireMachinaCollisionLogic(
    const SceneContext sceneContext)
    : Logic(sceneContext) {}

/////////////////////////////////////////////////
void GrimoireMachinaCollisionLogic::ProcessLogic() {

  // --- Asset section ---
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();
  MachinaFormScaffold *active_scaffold_form =
      grimoire_machina.m_scaffold_form.get();

  if (active_scaffold_form) {
    collision::mouse::ProcessScaffoldCollisions(
        *active_scaffold_form, m_scene_context.world_mouse_position);
  }
}

} // namespace steamrot::logic
