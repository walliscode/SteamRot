/////////////////////////////////////////////////
/// @file
/// @brief Declaration of UI helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "containers.h"

namespace steamrot::logic::ui {

/////////////////////////////////////////////////
/// @brief Update CUserInterface component visibility based on CUIState data
///
/// @param ui_state CUIState component containing visibility state data
/// @param scene_entities EntityMemoryPool containing scene entities
/////////////////////////////////////////////////
void UpdateCUserInterfaceVisibilityFromCUIState(
    CUIState &ui_state, EntityMemoryPool &scene_entities);

/////////////////////////////////////////////////
/// @brief Update CUserInterface root element disabled state based on CUIState
/// data
///
/// When all subscribers for a state key are active this function sets
/// is_disabled on the root UIElement of each listed CUserInterface entity.
/// Subscribers are reset to inactive afterwards, identical to the visibility
/// helper.
///
/// @param ui_state CUIState component containing disabled state data
/// @param scene_entities EntityMemoryPool containing scene entities
/////////////////////////////////////////////////
void UpdateUIDisabledStateFromCUIState(CUIState &ui_state,
                                       EntityMemoryPool &scene_entities);

} // namespace steamrot::logic::ui
