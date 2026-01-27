/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for SceneInfoEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneInfoEqualsMatcher.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
SceneInfoEqualsMatcher::SceneInfoEqualsMatcher(const SceneInfo &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool SceneInfoEqualsMatcher::match(const SceneInfo &actual) const {
  // m_mismatch_description.clear();
  // if (actual.type != m_expected.type) {
  //   m_mismatch_description +=
  //       "Scene type differs: actual='" +
  //       std::to_string(static_cast<int>(actual.type)) + "', expected='" +
  //       std::to_string(static_cast<int>(m_expected.type)) + "'; ";
  // }
  // if (actual.id != m_expected.id) {
  //   m_mismatch_description += "Scene ID differs; ";
  // }
  // return m_mismatch_description.empty();
  return false;
}

/////////////////////////////////////////////////
std::string SceneInfoEqualsMatcher::describe() const {
  return m_mismatch_description;
}

} // namespace steamrot::tests
