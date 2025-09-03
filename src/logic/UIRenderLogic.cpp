////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "Logic.h"
#include "draw_ui_elements.h"
#include "emp_helpers.h"
#include <SFML/Graphics.hpp>

namespace steamrot {

/////////////////////////////////////////////////////////////
UIRenderLogic::UIRenderLogic(const LogicContext logic_context)
    : Logic(logic_context) {}

////////////////////////////////////////////////////
void UIRenderLogic::ProcessLogic() {

  // Draw all UI elements to the render texture
  DrawUIElements();

  // display the render texture after drawing
  m_logic_context.scene_texture.display();
}

void UIRenderLogic::DrawUIElements() {

  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();

  const auto it = m_logic_context.archetypes.find(archetype_id);
  // if it is not in the archetypes map, then return
  if (it == m_logic_context.archetypes.end()) {
    return;
  }

  Archetype archetype = it->second;

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : archetype) {

    // get the CUserInterface component
    CUserInterface &ui_component = emp_helpers::GetComponent<CUserInterface>(
        entity_id, m_logic_context.scene_entities);

    draw_ui_elements::DrawNestedUIElements(
        m_logic_context.scene_texture, *ui_component.m_root_element,
        m_logic_context.asset_manager.GetDefaultUIStyle());
  }
}

} // namespace steamrot
