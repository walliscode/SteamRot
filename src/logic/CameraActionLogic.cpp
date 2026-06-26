/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CameraActionLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraActionLogic.h"
#include "action_camera.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
CameraActionLogic::CameraActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void CameraActionLogic::ProcessLogic() {

  // check if the GrimoireMachina exists in the scene context
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value())
    return;
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  action::camera::process_subscribers(m_subscribers,
                                      m_scene_context.camera_state,
                                      grimoire_machina.m_scaffold_form.get());
}
} // namespace steamrot::logic
