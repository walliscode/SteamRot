#include "UICollisionLogic.h"
#include "CUserInterface.h"
#include "archetypes.h"
#include "collision_mouse.h"
#include "entity_memory.h"
#include <SFML/Window/Mouse.hpp>

namespace steamrot::logic {
/////////////////////////////////////////////////
UICollisionLogic::UICollisionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UICollisionLogic::ProcessLogic() {

  // get entity indexes sorted by priority, highest first, so that
  // higher-priority entities claim the mouse before lower-priority ones
  std::vector<size_t> entity_indexes =
      archetypes::GetEntitiesSortedByPriority<CUserInterface>(
          m_scene_context.archetypes, m_scene_context.scene_entities,
          /*ascending=*/false);

  collision::mouse::ProcessUIEntityCollisions(
      entity_indexes, m_scene_context.scene_entities,
      m_scene_context.mouse_position);
}

} // namespace steamrot::logic
