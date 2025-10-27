#include "UICollisionLogic.h"
#include "ArchetypeUtils.h"
#include "CUserInterface.h"
#include "logic_collision.h"
#include "entity_memory.h"
#include <SFML/Window/Mouse.hpp>

namespace steamrot {
/////////////////////////////////////////////////
UICollisionLogic::UICollisionLogic(const SceneContext logic_context)
    : Logic(logic_context) {}

/////////////////////////////////////////////////
void UICollisionLogic::ProcessLogic() {

  // generate required archetype id
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();

  // check if archetype exists
  auto const it = m_scene_context.archetypes.find(archetype_id);

  // if archetype does not exist, we return
  if (it == m_scene_context.archetypes.end()) {
    return;
  }

  // get the archetype
  Archetype archetype = it->second;

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : archetype) {

    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    logic::collision::CheckMouseOverNestedUIElement(m_scene_context.mouse_position,
                                             *ui_component.m_root_element);
  };
}

} // namespace steamrot
