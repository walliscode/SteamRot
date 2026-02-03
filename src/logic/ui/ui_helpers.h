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
#include "CGrimoireMachina.h"
#include "containers.h"
#include <string>
#include <vector>

namespace steamrot::logic::ui {

/////////////////////////////////////////////////
/// @brief Get all available fragment names from CGrimoireMachina
///
/// @param grimoire_machina Reference to CGrimoireMachina component containing
/// fragments
/// @return Vector of fragment names
/////////////////////////////////////////////////
std::vector<std::string>
GetAllFragmentNames(const CGrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Get all available joint names from CGrimoireMachina
///
/// @param grimoire_machina Reference to CGrimoireMachina component containing
/// joints
/// @return Vector of joint names
/////////////////////////////////////////////////
std::vector<std::string>
GetAllJointNames(const CGrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Update CUserInterface component visibility based on CUIState data
///
/// @param ui_state CUIState component containing visibility state data
/// @param scene_entities EntityMemoryPool containing scene entities
/////////////////////////////////////////////////
void UpdateCUserInterfaceVisibilityFromCUIState(
    CUIState &ui_state, EntityMemoryPool &scene_entities);

} // namespace steamrot::logic::ui
