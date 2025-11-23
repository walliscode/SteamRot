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
    oss << conmat::Indent(1) << conmat::TestFailed() << "m_active:"
        << "\n";

    oss << conmat::Indent(2)
        << "actual: " << conmat::Colorize(actual.m_active, conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
  }

  if (actual.m_state_to_ui_visibility != m_expected.m_state_to_ui_visibility) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "m_state_to_ui_visibility mismatch:"
        << "\n";

    oss << conmat::Indent(2) << "actual: "
        << conmat::Colorize(actual.m_state_to_ui_visibility.size(),
                            conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_state_to_ui_visibility.size(),
                            conmat::Color::Blue)
        << "\n";
  }

  if (actual.m_state_values != m_expected.m_state_values) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "m_state_values:"
        << "\n";

    oss << conmat::Indent(2)
        << "actual: " << conmat::Colorize("differs", conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2)
        << "expected: " << conmat::Colorize("differs", conmat::Color::Blue)
        << "\n";
  }

  if (actual.m_state_subscribers.size() !=
      m_expected.m_state_subscribers.size()) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "m_state_subscribers size mismatch:"
        << "\n";

    oss << conmat::Indent(2) << "actual: "
        << conmat::Colorize(actual.m_state_subscribers.size(),
                            conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_state_subscribers.size(),
                            conmat::Color::Blue)
        << "\n";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

} // namespace steamrot::tests
