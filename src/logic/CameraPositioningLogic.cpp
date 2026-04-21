/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CameraPositioningLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraPositioningLogic.h"
#include "positioning_camera.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
CameraPositioningLogic::CameraPositioningLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void CameraPositioningLogic::ProcessLogic() {
  positioning::camera::apply_pan(m_scene_context.camera_state);
}

} // namespace steamrot::logic
