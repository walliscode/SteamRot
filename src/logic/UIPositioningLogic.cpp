/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIPositioningLogic.h"
#include "CUserInterface.h"
#include "Logic.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "positioning_ui.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
UIPositioningLogic::UIPositioningLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIPositioningLogic::ProcessLogic() { PositionUIElements(); }

/////////////////////////////////////////////////
void UIPositioningLogic::PositionUIElements() {

  // Generate entity indexes for entities with CUserInterface component
  auto entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(
          m_scene_context.archetypes, true);

  // cycle through all the entity indexes in the archetype
  for (size_t entity_id : entity_indexes) {

    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    // only position if the CUserInterface is visible
    if (ui_component.m_visible && ui_component.m_root_element) {
      positioning::ui::PositionNestedUIElements(
          *ui_component.m_root_element,
          m_scene_context.asset_manager.GetDefaultUIStyle());
    }
  }
}

} // namespace steamrot::logic
