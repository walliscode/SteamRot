/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIStateLogic class.
/////////////////////////////////////////////////

#include "UIStateLogic.h"
#include "CUIState.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "ui_helpers.h"

namespace steamrot {
/////////////////////////////////////////////////
UIStateLogic::UIStateLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIStateLogic::ProcessLogic() {

  // get all entity indexes with CUIState component
  std::set<size_t> entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CUIState>(
          m_scene_context.archetypes, true);

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : entity_indexes) {

    // grab the CUIState component
    CUIState &ui_state = entity::memory::GetComponent<CUIState>(
        entity_id, m_scene_context.scene_entities);

    // update the UI state
    logic::ui::UpdateCUserInterfaceVisibilityFromCUIState(
        ui_state, m_scene_context.scene_entities);
  }
}

} // namespace steamrot
