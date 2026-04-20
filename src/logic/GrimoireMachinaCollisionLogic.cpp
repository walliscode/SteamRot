/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GrimoireMachinaCollisionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaCollisionLogic.h"
#include "collision_grimoire_machina.h"
#include "collision_mouse.h"
#include <variant>

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

    // Check for collisions between the active ghost item and the active
    // scaffold
    if (!std::holds_alternative<std::monostate>(
            m_scene_context.mr_ghost.m_instance)) {
      collision::grimoire_machina::check_collisions_between_ghost_and_scaffold(
          *active_scaffold_form, m_scene_context.mr_ghost);
    }
  }
}

} // namespace steamrot::logic
