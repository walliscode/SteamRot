/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GrimoireMachinaPositioningLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaPositioningLogic.h"
#include "positioning_grimoire_machina.h"

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaPositioningLogic::GrimoireMachinaPositioningLogic(
    const SceneContext &scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GrimoireMachinaPositioningLogic::ProcessLogic() {

  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value())
    return;
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  // position the MachinaFormScaffold if active
  if (grimoire_machina.m_scaffold_form)
    positioning::grimoire_machina::position_machina_form_scaffold(
        grimoire_machina.m_scaffold_form->parts);
}
} // namespace steamrot::logic
