/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GhostActionLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostActionLogic.h"
#include "action_ghost.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
GhostActionLogic::GhostActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GhostActionLogic::ProcessLogic() {
  for (auto &subscriber : m_subscribers) {
    if (!subscriber->m_active)
      continue;
    action::ghost::ProcessSubscriber(*subscriber, m_scene_context.mr_ghost);
  }
}

} // namespace steamrot::logic
