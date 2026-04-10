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

  // --- Asset section ---
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  // --- Event section ---
  for (auto &subscriber : m_subscribers) {
    if (!subscriber->m_active)
      continue;
    action::grimoire_machina::ProcessSubscriber(*subscriber, grimoire_machina);
  }

  // --- Asset section: color update ---
  MachinaFormScaffold *active_scaffold_form =
      grimoire_machina.m_scaffold_form.get();
  if (active_scaffold_form)
    action::grimoire_machina::SetColor(active_scaffold_form->growth_point);
}
} // namespace steamrot::logic
