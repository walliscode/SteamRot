/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIStateLogic class.
/////////////////////////////////////////////////

#include "UIStateLogic.h"
#include "ArchetypeHelpers.h"
#include "CUIState.h"
#include "emp_helpers.h"

namespace steamrot {
/////////////////////////////////////////////////
UIStateLogic::UIStateLogic(const LogicContext logic_context)
    : Logic(logic_context) {}

/////////////////////////////////////////////////
void UIStateLogic::ProcessLogic() {

  // Generate archetype ID for CUIState components
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUIState>();

  const auto it = m_logic_context.archetypes.find(archetype_id);
  
  // If archetype doesn't exist, skip processing
  if (it == m_logic_context.archetypes.end()) {
    return;
  }

  const Archetype &archetype = it->second;

  // Process each entity with CUIState component
  for (size_t entity_id : archetype) {
    CUIState &ui_state = emp_helpers::GetComponent<CUIState>(
        entity_id, m_logic_context.scene_entities);

    // Check each subscriber and update corresponding state
    for (auto &[state_key, subscriber] : ui_state.m_state_subscribers) {
      if (subscriber && subscriber->IsActive()) {
        // Set the state to true when subscriber is active
        ui_state.m_state_values[state_key] = true;
        
        // Deactivate the subscriber after processing
        subscriber->SetInactive();
      }
    }
  }
}

} // namespace steamrot
