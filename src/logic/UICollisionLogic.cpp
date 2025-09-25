#include "UICollisionLogic.h"
#include "ArchetypeHelpers.h"
#include "CUserInterface.h"
#include "collision.h"
#include "emp_helpers.h"
#include <SFML/Window/Mouse.hpp>

namespace steamrot {
/////////////////////////////////////////////////
UICollisionLogic::UICollisionLogic(const LogicContext logic_context)
    : Logic(logic_context) {}

/////////////////////////////////////////////////
void UICollisionLogic::ProcessLogic() {

  // generate required archetype id
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();

  // check if archetype exists
  auto const it = m_logic_context.archetypes.find(archetype_id);

  // if archetype does not exist, we return
  if (it == m_logic_context.archetypes.end()) {
    return;
  }

  // get the archetype
  Archetype archetype = it->second;

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : archetype) {

    // get the CUserInterface component
    CUserInterface &ui_component = emp_helpers::GetComponent<CUserInterface>(
        entity_id, m_logic_context.scene_entities);

    collision::CheckMouseOverNestedUIElement(m_logic_context.mouse_position,
                                             *ui_component.m_root_element);
  };
}

} // namespace steamrot
