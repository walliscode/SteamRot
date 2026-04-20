/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CameraPositioningLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraPositioningLogic.h"
#include "action_camera.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
CameraPositioningLogic::CameraPositioningLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void CameraPositioningLogic::ProcessLogic() {
  action::camera::process_subscribers(m_subscribers,
                                      m_scene_context.camera_state);
}

} // namespace steamrot::logic
