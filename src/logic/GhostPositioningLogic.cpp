/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GhostPositioningLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostPositioningLogic.h"
#include "positioning_ghost.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
GhostPositioningLogic::GhostPositioningLogic(const SceneContext &scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GhostPositioningLogic::ProcessLogic() {
  positioning::ghost::ProcessRotateSubscribers(m_subscribers,
                                               m_scene_context.mr_ghost);

  positioning::ghost::UpdatePosition(
      m_scene_context.mr_ghost, m_scene_context.world_mouse_position,
      m_scene_context.mouse_position, m_scene_context.camera_state,
      m_scene_context.scene_texture);
}

} // namespace steamrot::logic
