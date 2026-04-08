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

  // track whether any higher-priority entity has already claimed the mouse
  bool higher_priority_claimed_mouse = false;

  for (size_t entity_id : entity_indexes) {
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    // skip if not visible
    if (!ui_component.m_visible) {
      continue;
    }

    if (higher_priority_claimed_mouse) {
      // a higher-priority entity owns the mouse: clear hover state so this
      // entity does not respond to collision or actions
      collision::mouse::ClearMouseOver(*ui_component.m_root_element);
    } else {
      collision::mouse::CheckMouseOver(m_scene_context.mouse_position,
                                       *ui_component.m_root_element);

      // if this entity is now hovered, block all lower-priority entities
      if (collision::mouse::AnyMouseOver(*ui_component.m_root_element)) {
        higher_priority_claimed_mouse = true;
      }
    }
  }
}

} // namespace steamrot::logic
