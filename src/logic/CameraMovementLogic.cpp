/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CameraMovementLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraMovementLogic.h"
#include "action_camera.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
CameraMovementLogic::CameraMovementLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void CameraMovementLogic::ProcessLogic() {
  action::camera::ProcessSubscribers(m_subscribers,
                                     m_scene_context.camera_state);
}

} // namespace steamrot::logic
