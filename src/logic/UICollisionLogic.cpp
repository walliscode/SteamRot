#include "UICollisionLogic.h"
#include "CUserInterface.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "collision_mouse.h"
#include <SFML/Window/Mouse.hpp>

namespace steamrot::logic {
/////////////////////////////////////////////////
UICollisionLogic::UICollisionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UICollisionLogic::ProcessLogic() {

  // get all entity indexes with CUserInterface component
  std::set<size_t> entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(
          m_scene_context.archetypes, true);

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : entity_indexes) {
    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    // skip if not visible
    if (!ui_component.m_visible) {
      continue;
    }
    collision::mouse::CheckMouseOver(
        m_scene_context.mouse_position, *ui_component.m_root_element);
  };
}

} // namespace steamrot::logic
