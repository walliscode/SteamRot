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
    CUIState &ui_state, EntityMemoryPool &scene_entities) {

  // cycle map of subscriber vectors, for a state to be considered, all
  // subscribers must be active
  for (const auto &[state_key, subscriber_vec] : ui_state.m_state_subscribers) {

    // bool to track if all subscribers are active
    bool all_active = true;
    for (const auto &subscriber : subscriber_vec) {
      if (!subscriber->m_active) {
        all_active = false;
        // break early if any subscriber is inactive
        break;
      }
    }

    // if all subscribers are active, set UI components on/off accordingly
    if (all_active) {

      // check state key exists in state_values map, if not, skip
      auto state_value_it = ui_state.m_state_values.find(state_key);
      if (state_value_it == ui_state.m_state_values.end()) {
        continue;
      }

      // flip state value and use the new value to toggle UI components
      bool &state_value = state_value_it->second;
      state_value = !state_value;

      // attempt to find UI visibility state for this state key, if not found,
      // skip
      auto ui_visibility_it = ui_state.m_state_to_ui_visibility.find(state_key);
      if (ui_visibility_it == ui_state.m_state_to_ui_visibility.end())
        continue;

      const UIVisibilityState &ui_visibility_state = ui_visibility_it->second;
      // toggle UI components
      for (const size_t ui_index_on : ui_visibility_state.m_ui_indices_on) {
        CUserInterface &ui_component =
            entity::memory::GetComponent<CUserInterface>(ui_index_on,
                                                         scene_entities);

        ui_component.m_visible = true;
      }
      for (const size_t ui_index_off : ui_visibility_state.m_ui_indices_off) {
        CUserInterface &ui_component =
            entity::memory::GetComponent<CUserInterface>(ui_index_off,
                                                         scene_entities);

        ui_component.m_visible = false;
      }
    }

    // finally, reset all subscribers to inactive for next check
    for (const auto &subscriber : subscriber_vec) {
      subscriber->m_active = false;
    }
  }
}

} // namespace steamrot::logic::ui
