#include "UICollisionLogic.h"
#include "ArchetypeUtils.h"
#include "CUserInterface.h"
#include "logic_collision.h"
#include "entity_memory.h"
#include <SFML/Window/Mouse.hpp>

namespace steamrot {
/////////////////////////////////////////////////
UICollisionLogic::UICollisionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UICollisionLogic::ProcessLogic() {

  // Gather entity indices using the new archetype gathering functionality
  // Using exact_match=true to get only entities with exactly CUserInterface
  std::set<size_t> entity_indices = 
      GatherEntityIndices<CUserInterface>(m_scene_context.archetypes, true);

  // cycle through all the entity indices
  for (size_t entity_id : entity_indices) {

    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    logic::collision::CheckMouseOverNestedUIElement(m_scene_context.mouse_position,
                                             *ui_component.m_root_element);
  };
}

} // namespace steamrot
