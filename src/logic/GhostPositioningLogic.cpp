/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GhostPositioningLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostPositioningLogic.h"
#include "positioning_ghost.h"
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic {

/////////////////////////////////////////////////
GhostPositioningLogic::GhostPositioningLogic(const SceneContext &scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GhostPositioningLogic::ProcessLogic() {
  positioning::ghost::UpdatePosition(m_scene_context.mr_ghost,
                                     m_scene_context.mouse_position);
}

} // namespace steamrot::logic
