/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for CMeta comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CMetaEqualsMatcher.h"
#include "conmat.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::string CMetaEqualsMatcher::GetComponentName() const { return "CMeta"; }

/////////////////////////////////////////////////
CMetaEqualsMatcher::CMetaEqualsMatcher(const CMeta &expected)
    : ComponentMatcherBase<CMeta>(expected) {}

/////////////////////////////////////////////////
bool CMetaEqualsMatcher::match(const CMeta &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  if (actual.m_active != m_expected.m_active) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "m_active:"
        << "\n";

    oss << conmat::Indent(2)
        << "actual: " << conmat::Colorize(actual.m_active, conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
  }

  if (actual.m_entity_active != m_expected.m_entity_active) {

    oss << conmat::Indent(1) << conmat::TestFailed() << "m_entity_active:"
        << "\n";
    oss << conmat::Indent(2) << "actual: "
        << conmat::Colorize(actual.m_entity_active, conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_entity_active, conmat::Color::Blue)
        << "\n";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

} // namespace steamrot::tests
