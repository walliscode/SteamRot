/////////////////////////////////////////////////
/// @file
/// @brief Implementation of UI helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "logic_ui.h"
#include "CUIState.h"
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
  //
  // // cycle through Subscribers
  // for (const auto &[state_key, subscriber_vec] :
  // ui_state.m_state_subscribers) {
  //
  //   // all Subscribers for this state key need be active to turn on UI
  //   bool all_active = true;
  //
  //   // cycle through and if any are inactive, set all_active to false
  //   for (const auto &subscriber : subscriber_vec) {
  //     if (!subscriber->IsActive()) {
  //       all_active = false;
  //       // break out early if any are inactive
  //       break;
  //     }
  //   }
  //
  //   // continue to to next state key if all active is false
  //   if (!all_active)
  //     continue;
  //
  //   // get the state value for this state key
  //   bool state_value = ui_state.m_state_values.at(state_key);
  //
  //   /////////////////////////////////////////////////
  //   /// TOGGLE SYSTEM EXPLAINED
  //   /// If state_value is true, we want to set the UI components in the 'on'
  //   /// list to on (visible) and those in the 'off' list to off (invisible).
  //   If
  //   /// state_value is false, we want to set the UI components in the 'on
  //   list
  //   /// to off (invisible) and those in the 'off' list to on (visible). This
  //   is
  //   /// achieved by using the state_value combined with a boolean AND
  //   operation.
  //   /////////////////////////////////////////////////
  //
  //   // get the UI visibility state for this state key
  //   auto it = ui_state.m_state_to_ui_visibility.find(state_key);
  //   if (it == ui_state.m_state_to_ui_visibility.end()) {
  //     continue; // no visibility state found for this key
  //   }
  //   const UIVisibilityState &ui_visibility_state = it->second;
  //
  //   // update UI components based on state value
  //   for (size_t ui_index : ui_visibility_state.m_ui_indices_on) {
  //
  //     CUserInterface &ui_component =
  //         entity::memory::GetComponent<CUserInterface>(ui_index,
  //                                                      scene_entities);
  //
  //     // toggle, see explanation above
  //     ui_component.m_is_visible = state_value && true;
  //   }
  //   for (size_t ui_index : ui_visibility_state.m_ui_indices_off) {
  //     CUserInterface &ui_component =
  //         entity::memory::GetComponent<CUserInterface>(ui_index,
  //                                                      scene_entities);
  //     // toggle, see explanation above
  //     ui_component.m_is_visible = state_value && false;
  //   }
  // }
}

} // namespace steamrot::logic::ui
