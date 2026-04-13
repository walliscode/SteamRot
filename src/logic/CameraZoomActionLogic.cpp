/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CameraZoomActionLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraZoomActionLogic.h"
#include "action_camera.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
CameraZoomActionLogic::CameraZoomActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void CameraZoomActionLogic::ProcessLogic() {
  action::camera::ProcessSubscribers(m_subscribers,
                                     m_scene_context.camera_state);
}

} // namespace steamrot::logic
