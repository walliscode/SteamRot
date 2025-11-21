/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for CMeta comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "cmeta_matchers.h"
#include "conmat.h"

namespace steamrot::tests {
/////////////////////////////////////////////////
CMetaEqualsMatcher::CMetaEqualsMatcher(const CMeta &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool CMetaEqualsMatcher::match(const CMeta &actual) const {
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

  if (actual.m_entity_active != m_expected.m_entity_active) {

    oss << conmat::Indent(1) << conmat::TestFailed()
        << "m_entity_active:" << "\n";
    oss << conmat::Indent(2) << "actual: "
        << conmat::Colorize(actual.m_entity_active, conmat::Color::Red) << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_entity_active, conmat::Color::Blue)
        << "\n";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string CMetaEqualsMatcher::describe() const {
  // if mismatch description is empty then we can assume the test passed
  if (m_mismatch_description.empty()) {

    std::ostringstream oss;
    oss << conmat::Header(conmat::TestPassed() + "CMeta Match:", 3) << "\n";

    return oss.str();

  } else {

    std::ostringstream oss;
    oss << conmat::Header(conmat::TestFailed() + "CMeta Match:", 3) << "\n";
    oss << m_mismatch_description;

    return oss.str();
  }
}
} // namespace steamrot::tests
