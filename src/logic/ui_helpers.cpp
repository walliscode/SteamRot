
/// @file
/// @brief Implementation of UI helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ui_helpers.h"
#include "CUserInterface.h"
#include "entity_memory.h"

namespace steamrot::logic::ui {

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

/////////////////////////////////////////////////
void UpdateUIDisabledStateFromCUIState(CUIState &ui_state,
                                       EntityMemoryPool &scene_entities) {

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

    // if all subscribers are active, apply disabled state changes
    if (all_active) {
      // attempt to find UI disabled state for this state key, if not found,
      // skip
      auto ui_disabled_it = ui_state.m_state_to_ui_disabled.find(state_key);
      if (ui_disabled_it == ui_state.m_state_to_ui_disabled.end())
        continue;
      const UIDisabledState &ui_disabled_state = ui_disabled_it->second;

      // disable root elements
      for (const size_t ui_index : ui_disabled_state.m_ui_indices_disabled) {
        CUserInterface &ui_component =
            entity::memory::GetComponent<CUserInterface>(ui_index,
                                                         scene_entities);
        if (ui_component.m_root_element) {
          ui_component.m_root_element->is_disabled = true;
        }
      }

      // enable root elements
      for (const size_t ui_index : ui_disabled_state.m_ui_indices_enabled) {
        CUserInterface &ui_component =
            entity::memory::GetComponent<CUserInterface>(ui_index,
                                                         scene_entities);
        if (ui_component.m_root_element) {
          ui_component.m_root_element->is_disabled = false;
        }
      }
    }

    // NOTE: subscribers are NOT reset here. UIStateLogic calls this helper
    // before UpdateCUserInterfaceVisibilityFromCUIState, which is responsible
    // for resetting subscribers after processing visibility changes.
  }
}

} // namespace steamrot::logic::ui
