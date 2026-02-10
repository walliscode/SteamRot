/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "Logic.h"
#include "UIRenderContext.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "ui_render.h"
#include <SFML/Graphics.hpp>

namespace steamrot {

/////////////////////////////////////////////////
UIRenderLogic::UIRenderLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIRenderLogic::ProcessLogic() {

  // Draw all UI elements to the render texture
  DrawUIElements();

  // display the render texture after drawing
  // m_scene_context.scene_texture.display();
}

void UIRenderLogic::DrawUIElements() {

  // print out number of archetypes with number of entities for debugging

  for (const auto &[archetype_id, entity_indices] :
       m_scene_context.archetypes) {
  }
  // Generate entity indexes for entities with only CUserInterface component
  auto entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(

          m_scene_context.archetypes, true);

  // Create render context with texture and default style
  logic::ui::render::UIRenderContext render_context{
      m_scene_context.scene_texture,
      m_scene_context.asset_manager.GetDefaultUIStyle()};

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : entity_indexes) {

    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    // only draw if the CUserInterface is visible
    if (ui_component.m_visible) {
      logic::ui::render::DrawNestedUIElements(render_context,
                                              *ui_component.m_root_element);
    }
  }
}

} // namespace steamrot
