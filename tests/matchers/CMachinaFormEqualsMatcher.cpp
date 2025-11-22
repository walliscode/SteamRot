/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for CMachinaForm comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CMachinaFormEqualsMatcher.h"
#include "conmat.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::string CMachinaFormEqualsMatcher::GetComponentName() const {
  return "CMachinaForm";
}

/////////////////////////////////////////////////
CMachinaFormEqualsMatcher::CMachinaFormEqualsMatcher(
    const CMachinaForm &expected)
    : ComponentMatcherBase<CMachinaForm>(expected) {}

/////////////////////////////////////////////////
bool CMachinaFormEqualsMatcher::match(const CMachinaForm &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  if (actual.m_active != m_expected.m_active) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "m_active:" << "\n";

    oss << conmat::Indent(2)
        << "actual: " << conmat::Colorize(actual.m_active, conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
  }

  if (actual.m_fragments.size() != m_expected.m_fragments.size()) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "m_fragments size mismatch:" << "\n";
    oss << conmat::Indent(2) << "actual size: "
        << conmat::Colorize(actual.m_fragments.size(), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected size: "
        << conmat::Colorize(m_expected.m_fragments.size(), conmat::Color::Blue)
        << "\n";
  }

  if (actual.m_joints.size() != m_expected.m_joints.size()) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "m_joints size mismatch:" << "\n";
    oss << conmat::Indent(2) << "actual size: "
        << conmat::Colorize(actual.m_joints.size(), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected size: "
        << conmat::Colorize(m_expected.m_joints.size(), conmat::Color::Blue)
        << "\n";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

} // namespace steamrot::tests
