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

  // get THE grimoire machina from the AssetManager
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();
  MachinaFormScaffold *active_scaffold_form =
      grimoire_machina.m_scaffold_form.get();

  // run collision logic for all parts of the active MachinaFormScaffold
  if (active_scaffold_form) {

    collision::mouse::CheckMouseOver(m_scene_context.mouse_position,
                                     active_scaffold_form->growth_point);

    for (auto &joint : active_scaffold_form->joints) {
      collision::mouse::CheckMouseOver(m_scene_context.mouse_position, joint);
    }

    for (auto &fragment : active_scaffold_form->fragments) {
      collision::mouse::CheckMouseOver(m_scene_context.mouse_position,
                                       fragment);
    }
  }
}

} // namespace steamrot::logic
