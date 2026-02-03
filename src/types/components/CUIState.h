/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CUIState class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Component.h"
#include "Subscriber.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Holds UI visibility state for a particular state key
///
/// Specifies which CUserInterface components should be visible (on)
/// which should be hidden (off) for a given state.
/////////////////////////////////////////////////
struct UIVisibilityState {
  /////////////////////////////////////////////////
  /// @brief Entity indices of UI components that should be visible
  /////////////////////////////////////////////////
  std::vector<size_t> m_ui_indices_on;

  /////////////////////////////////////////////////
  /// @brief Entity indices of UI components that should be hidden
  /////////////////////////////////////////////////
  std::vector<size_t> m_ui_indices_off;
};

/////////////////////////////////////////////////
/// @brief Component for managing UI visibility based on scene state
///
/// This component maps state keys to UI visibility states,
/// specifying which UI components should be on and which should be off.
/////////////////////////////////////////////////
struct CUIState : public Component {

  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  CUIState() = default;

  /////////////////////////////////////////////////
  /// @brief Mapping of state keys to UI visibility states
  ///
  /// Each state key maps to a UIVisibilityState that specifies
  /// which UI components should be visible (on) and which should
  /// be hidden (off).
  /////////////////////////////////////////////////
  std::unordered_map<std::string, UIVisibilityState> m_state_to_ui_visibility;

  /////////////////////////////////////////////////
  /// @brief Mapping of state keys to their associated subscribers
  ///
  /// Each state key can have multiple subscribers. The state will only
  /// be set to true when ALL subscribers are activated (AND logic).
  /////////////////////////////////////////////////
  std::unordered_map<std::string, std::vector<std::shared_ptr<Subscriber>>>
      m_state_subscribers;
};

} // namespace steamrot
