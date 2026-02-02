/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "Logic.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "logic_render.h"
#include <SFML/Graphics.hpp>

namespace steamrot {

/////////////////////////////////////////////////
UIRenderLogic::UIRenderLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIRenderLogic::ProcessLogic() {

  // Draw all UI elements to the render texture
  DrawUIElements();

  // NOTE: display() is now called by the Scene's sRender() method
  // after all render logic has completed, not here.
}

void UIRenderLogic::DrawUIElements() {

  // Generate entity indexes for entities with only CUserInterface component
  auto entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(

          m_scene_context.archetypes, true);

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : entity_indexes) {

    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    // only draw if the CUserInterface is visible
    if (ui_component.m_visible) {
      logic::render::DrawNestedUIElements(
          m_scene_context.scene_texture, *ui_component.m_root_element,
          m_scene_context.asset_manager.GetDefaultUIStyle());
    }
  }
}

} // namespace steamrot
