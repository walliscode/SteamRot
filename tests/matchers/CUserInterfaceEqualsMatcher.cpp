/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for CUserInterface comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CUserInterfaceEqualsMatcher.h"
#include "conmat.h"
#include "ui_element_matchers.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::string CUserInterfaceEqualsMatcher::GetComponentName() const {
  return "CUserInterface";
}

/////////////////////////////////////////////////
CUserInterfaceEqualsMatcher::CUserInterfaceEqualsMatcher(
    const CUserInterface &expected)
    : ComponentMatcherBase<CUserInterface>(expected) {}

/////////////////////////////////////////////////
bool CUserInterfaceEqualsMatcher::match(const CUserInterface &actual) const {
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

  if (actual.m_name != m_expected.m_name) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "m_name:"
        << "\n";

    oss << conmat::Indent(2)
        << "actual: " << conmat::Colorize(actual.m_name, conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_name, conmat::Color::Blue) << "\n";
  }

  if (actual.m_UI_visible != m_expected.m_UI_visible) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "m_UI_visible:"
        << "\n";

    oss << conmat::Indent(2)
        << "actual: "
        << conmat::Colorize(actual.m_UI_visible, conmat::Color::Red) << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_UI_visible, conmat::Color::Blue)
        << "\n";
  }

  // Check root element pointers - both null or both non-null
  bool both_null = (actual.m_root_element == nullptr &&
                    m_expected.m_root_element == nullptr);
  bool both_non_null = (actual.m_root_element != nullptr &&
                        m_expected.m_root_element != nullptr);

  if (!both_null && !both_non_null) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "m_root_element:"
        << "\n";

    if (actual.m_root_element == nullptr) {
      oss << conmat::Indent(2)
          << "actual: " << conmat::Colorize("nullptr", conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize("non-nullptr", conmat::Color::Blue) << "\n";
    } else {
      oss << conmat::Indent(2)
          << "actual: "
          << conmat::Colorize("non-nullptr", conmat::Color::Red) << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize("nullptr", conmat::Color::Blue) << "\n";
    }
  } else if (both_non_null) {
    // Both are non-null, use UIElement matcher to compare them recursively
    UIElementEqualsMatcher element_matcher(*m_expected.m_root_element);
    if (!element_matcher.match(*actual.m_root_element)) {

      oss << "  " << conmat::TestFailed() << "m_root_element differences:"
          << "\n";
      std::istringstream element_diff_stream(element_matcher.describe());
      std::string line;
      while (std::getline(element_diff_stream, line)) {
        oss << line << "\n";
      }
    }
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

} // namespace steamrot::tests
