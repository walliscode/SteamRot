/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for CUIState comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CUIStateEqualsMatcher.h"
#include "conmat.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::string CUIStateEqualsMatcher::GetComponentName() const {
  return "CUIState";
}

/////////////////////////////////////////////////
CUIStateEqualsMatcher::CUIStateEqualsMatcher(const CUIState &expected)
    : ComponentMatcherBase<CUIState>(expected) {}

/////////////////////////////////////////////////
bool CUIStateEqualsMatcher::match(const CUIState &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  if (actual.m_active != m_expected.m_active) {
    oss << "\t" << conmat::TestFailed() << "m_active:"
        << "\n";
    oss << "\t\t"
        << "actual = " << conmat::Colorize(actual.m_active, conmat::Color::Red)
        << "\n";
    oss << "\t\t"
        << "expected = "
        << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
  }

  if (actual.m_state_to_ui_visibility.size() !=
      m_expected.m_state_to_ui_visibility.size()) {
    oss << "\t" << conmat::TestFailed()
        << "m_state_to_ui_visibility size mismatch:"
        << "\n";
    oss << "\t\t"
        << "actual size = " << actual.m_state_to_ui_visibility.size() << "\n";
    oss << "\t\t"
        << "expected size = " << m_expected.m_state_to_ui_visibility.size()
        << "\n";
  }

  if (actual.m_state_values != m_expected.m_state_values) {
    oss << "\t" << conmat::TestFailed() << "m_state_values differs"
        << "\n";
  }

  if (actual.m_state_subscribers.size() !=
      m_expected.m_state_subscribers.size()) {
    oss << "\t" << conmat::TestFailed()
        << "m_state_subscribers size mismatch:"
        << "\n";
    oss << "\t\t"
        << "actual size = " << actual.m_state_subscribers.size() << "\n";
    oss << "\t\t"
        << "expected size = " << m_expected.m_state_subscribers.size() << "\n";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

} // namespace steamrot::tests
