/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "CUserInterface.h"
#include "Logic.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "render_ui.h"
#include <SFML/Graphics.hpp>

namespace steamrot::logic {

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
  // Generate entity indexes sorted by priority ascending so that
  // higher-priority entities are drawn last (on top)
  auto entity_indexes =
      archetypes::GetEntitiesSortedByPriority<CUserInterface>(
          m_scene_context.archetypes, m_scene_context.scene_entities,
          /*ascending=*/true);

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : entity_indexes) {

    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    // only draw if the CUserInterface is visible
    if (ui_component.m_visible) {
      render::ui::DrawNestedUIElements(
          m_scene_context.scene_texture, *ui_component.m_root_element,
          m_scene_context.asset_manager.GetDefaultUIStyle());
    }
  }
}

} // namespace steamrot::logic
