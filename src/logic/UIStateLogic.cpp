/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIStateLogic class.
/////////////////////////////////////////////////

#include "UIStateLogic.h"
#include "CUIState.h"
#include "archetype_helpers.h"
#include "entity_memory.h"

namespace steamrot {
/////////////////////////////////////////////////
UIStateLogic::UIStateLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIStateLogic::ProcessLogic() {

  // get all entity indexes with CUIState component
  std::set<size_t> entity_indexes =
      GenerateEntityIndexesFromComponents<CUIState>(m_scene_context.archetypes,
                                                    true);

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : entity_indexes) {
    CUIState &ui_state = entity::memory::GetComponent<CUIState>(
        entity_id, m_scene_context.scene_entities);

    // Check all subscribers for each state
    for (auto &[state_key, subscribers] : ui_state.m_state_subscribers) {
      // Skip if no subscribers
      if (subscribers.empty()) {
        continue;
      }

      // Check if ALL subscribers are active (AND logic)
      bool all_active = true;
      for (auto &subscriber : subscribers) {
        if (!subscriber || !subscriber->IsActive()) {
          all_active = false;
          break;
        }
      }

      // Set the state to true only if ALL subscribers are active
      if (all_active) {
        ui_state.m_state_values[state_key] = true;

        // Deactivate all subscribers after processing
        for (auto &subscriber : subscribers) {
          subscriber->SetInactive();
        }
      }
    }
  }
}

} // namespace steamrot
