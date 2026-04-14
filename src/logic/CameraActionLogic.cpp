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
  action::camera::ProcessSubscribers(m_subscribers,
                                     m_scene_context.camera_state);
}

} // namespace steamrot::logic
