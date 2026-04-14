/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GrimoireMachinaPositioningLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaPositioningLogic.h"

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaPositioningLogic::GrimoireMachinaPositioningLogic(
    const SceneContext &scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GrimoireMachinaPositioningLogic::ProcessLogic() {
  // Intentionally empty: the crafting canvas has been replaced with an
  // infinite canvas, so there is nothing to compute here.  This class is
  // retained to preserve LogicFactory and LogicClassEnum compatibility.
}
} // namespace steamrot::logic
