/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GrimoireMachinaCollisionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaCollisionLogic.h"
#include "collision_grimoire_machina.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
GrimoireMachinaCollisionLogic::GrimoireMachinaCollisionLogic(
    const SceneContext sceneContext)
    : Logic(sceneContext) {}

/////////////////////////////////////////////////
void GrimoireMachinaCollisionLogic::ProcessLogic() {

  // get THE grimoire machina from the AssetManager
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();
  MachinaFormScaffold *active_scaffold_form =
      grimoire_machina.m_scaffold_form.get();

  // run logic based MachinaFormScaffold
  if (active_scaffold_form) {

    collision::grimoire_machina::CheckMouseOverGrowthPoint(
        m_scene_context.mouse_position, active_scaffold_form->growth_point);
  }
}

} // namespace steamrot::logic
