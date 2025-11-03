////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "Logic.h"
#include "draw_ui_elements.h"
#include "entity_memory.h"
#include <SFML/Graphics.hpp>

namespace steamrot {

/////////////////////////////////////////////////////////////
UIRenderLogic::UIRenderLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

////////////////////////////////////////////////////
void UIRenderLogic::ProcessLogic() {

  // Draw all UI elements to the render texture
  DrawUIElements();

  // display the render texture after drawing
  m_scene_context.scene_texture.display();
}

void UIRenderLogic::DrawUIElements() {

  // Gather entity indices using the new archetype gathering functionality
  // Using exact_match=true to get only entities with exactly CUserInterface
  std::set<size_t> entity_indices = 
      GatherEntityIndices<CUserInterface>(m_scene_context.archetypes, true);

  // cycle through all the entity indices
  for (size_t entity_id : entity_indices) {

    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    draw_ui_elements::DrawNestedUIElements(
        m_scene_context.scene_texture, *ui_component.m_root_element,
        m_scene_context.asset_manager.GetDefaultUIStyle());
  }
}

} // namespace steamrot
