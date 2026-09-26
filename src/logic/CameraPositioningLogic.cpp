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
  positioning::camera::update_world_mouse_position(
      m_scene_context.world_mouse_position, m_scene_context.camera_state,
      m_scene_context.mouse_position, m_scene_context.scene_texture);
}

} // namespace steamrot::logic
