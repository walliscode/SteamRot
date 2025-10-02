/////////////////////////////////////////////////
/// @file
/// @brief Declaration of entity manipulation helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachina.h"
#include <string>
#include <vector>

namespace steamrot::entity_helpers {

/////////////////////////////////////////////////
/// @brief Get all available fragment names from CGrimoireMachina
///
/// @param grimoire_machina Reference to CGrimoireMachina component containing fragments
/// @return Vector of fragment names
/////////////////////////////////////////////////
std::vector<std::string>
GetAllFragmentNames(const CGrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Get all available joint names from CGrimoireMachina
///
/// @param grimoire_machina Reference to CGrimoireMachina component containing joints
/// @return Vector of joint names
/////////////////////////////////////////////////
std::vector<std::string>
GetAllJointNames(const CGrimoireMachina &grimoire_machina);

} // namespace steamrot::entity_helpers
