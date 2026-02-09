/////////////////////////////////////////////////
/// @file
/// @brief Implementation of GrimoireMachinaActionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaActionLogic.h"

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaActionLogic::GrimoireMachinaActionLogic(
    const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GrimoireMachinaActionLogic::ProcessLogic() {

  // get THE grimoire machina from the AssetManager
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  // run through functions
}
} // namespace steamrot::logic
