/////////////////////////////////////////////////
/// @file
/// @brief Implementation of UI helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "logic_ui.h"
#include "CUserInterface.h"
#include "entity_memory.h"

namespace steamrot::logic::ui {

/////////////////////////////////////////////////
std::vector<std::string>
GetAllFragmentNames(const CGrimoireMachina &grimoire_machina) {
  std::vector<std::string> fragment_names;
  fragment_names.reserve(grimoire_machina.m_all_fragments.size());

  for (const auto &[name, fragment] : grimoire_machina.m_all_fragments) {
    fragment_names.push_back(name);
  }

  return fragment_names;
}

/////////////////////////////////////////////////
std::vector<std::string>
GetAllJointNames(const CGrimoireMachina &grimoire_machina) {
  std::vector<std::string> joint_names;
  joint_names.reserve(grimoire_machina.m_all_joints.size());

  for (const auto &[name, joint] : grimoire_machina.m_all_joints) {
    joint_names.push_back(name);
  }

  return joint_names;
}

/////////////////////////////////////////////////
void UpdateCUserInterfaceVisibilityFromCUIState(
    const CUIState &ui_state, EntityMemoryPool &scene_entities) {

  // find the states that are currently active
  std::vector<std::string> active_states;
  for (const auto &[state_key, is_active] : ui_state.m_state_values) {
    if (is_active) {
      active_states.push_back(state_key);
    }
  }

  // cycle through active states and update UI visibility
  for (const std::string &state_key : active_states) {

    const auto it = ui_state.m_state_to_ui_visibility.find(state_key);

    if (it != ui_state.m_state_to_ui_visibility.end()) {
      const UIVisibilityState &visibility_state = it->second;
      // Update UI components to be visible (on)
      for (size_t ui_index : visibility_state.m_ui_indices_on) {
        auto &ui_component = entity::memory::GetComponent<CUserInterface>(
            ui_index, scene_entities);
        ui_component.m_UI_visible = true;
      }
      // Update UI components to be hidden (off)
      for (size_t ui_index : visibility_state.m_ui_indices_off) {
        auto &ui_component = entity::memory::GetComponent<CUserInterface>(
            ui_index, scene_entities);
        ui_component.m_UI_visible = false;
      }
    }
  }

  // Finally update
}

} // namespace steamrot::logic::ui
