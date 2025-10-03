/////////////////////////////////////////////////
/// @file
/// @brief Implementation of UI helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ui_helpers.h"

namespace steamrot::ui_helpers {

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

} // namespace steamrot::ui_helpers
