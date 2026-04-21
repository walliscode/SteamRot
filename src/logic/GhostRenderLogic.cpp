/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GhostRenderLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostRenderLogic.h"
#include "positioning_camera.h"
#include "render_ghost.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
GhostRenderLogic::GhostRenderLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GhostRenderLogic::ProcessLogic() {

  // Apply the world (zoomed) view so the ghost is positioned in world space.
  m_scene_context.scene_texture.setView(positioning::camera::get_world_view(
      m_scene_context.camera_state, m_scene_context.scene_texture,
      m_scene_context.scene_state.scene_type, m_scene_context.scene_entities));

  render::ghost::draw_ghost_item(m_scene_context.scene_texture,
                                 m_scene_context.mr_ghost);
}

} // namespace steamrot::logic
