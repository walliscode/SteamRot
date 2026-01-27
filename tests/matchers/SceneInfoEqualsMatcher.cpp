/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for SceneInfoEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneInfoEqualsMatcher.h"
#include "conmat.h"
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
SceneInfoEqualsMatcher::SceneInfoEqualsMatcher(const SceneInfo &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool SceneInfoEqualsMatcher::match(const SceneInfo &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  if (actual.type != m_expected.type) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "Scene type differs:" << "\n";
    oss << conmat::Indent(2)
        << "actual: " << conmat::Colorize(EnumNameSceneType(actual.type),
                                          conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(EnumNameSceneType(m_expected.type),
                            conmat::Color::Blue)
        << "\n";
  }

  if (actual.id != m_expected.id) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "Scene ID differs:" << "\n";
    oss << conmat::Indent(2)
        << "actual: " << conmat::Colorize(uuids::to_string(actual.id),
                                          conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(uuids::to_string(m_expected.id),
                            conmat::Color::Blue)
        << "\n";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string SceneInfoEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    return conmat::Header(conmat::TestPassed() + "SceneInfo Match:", 3) + "\n";
  } else {
    return conmat::Header(conmat::TestFailed() + "SceneInfo Mismatch:", 3) +
           "\n" + m_mismatch_description;
  }
}

} // namespace steamrot::tests
