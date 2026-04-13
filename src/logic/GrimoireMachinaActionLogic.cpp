/////////////////////////////////////////////////
/// @file
/// @brief Implementation of GrimoireMachinaActionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaActionLogic.h"
#include "action_grimoire_machina.h"

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaActionLogic::GrimoireMachinaActionLogic(
    const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GrimoireMachinaActionLogic::ProcessLogic() {

  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value())
    return;
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  action::grimoire_machina::ProcessSubscribers(m_subscribers, m_scene_context,
                                               grimoire_machina);
}
} // namespace steamrot::logic
