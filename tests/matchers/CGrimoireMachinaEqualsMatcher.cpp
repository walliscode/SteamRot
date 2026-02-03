/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for CGrimoireMachina comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachinaEqualsMatcher.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
CGrimoireMachinaEqualsMatcher::CGrimoireMachinaEqualsMatcher(
    const CGrimoireMachina &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
std::string CGrimoireMachinaEqualsMatcher::GetHeader() const {
  std::string status =
      m_mismatch_description.empty() ? conmat::TestPassed() : conmat::TestFailed();
  return conmat::Header(status + " CGrimoireMachina Match:", 3) + "\n";
}

/////////////////////////////////////////////////
bool CGrimoireMachinaEqualsMatcher::match(
    const CGrimoireMachina &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  if (actual.m_all_fragments.size() != m_expected.m_all_fragments.size()) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "m_all_fragments size mismatch:"
        << "\n";

    oss << conmat::Indent(2) << "actual: "
        << conmat::Colorize(actual.m_all_fragments.size(), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_all_fragments.size(),
                            conmat::Color::Blue)
        << "\n";
  }

  if (actual.m_all_joints.size() != m_expected.m_all_joints.size()) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "m_all_joints size mismatch:"
        << "\n";

    oss << conmat::Indent(2) << "actual: "
        << conmat::Colorize(actual.m_all_joints.size(), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_all_joints.size(),
                            conmat::Color::Blue)
        << "\n";
  }

  if (actual.m_machina_forms.size() != m_expected.m_machina_forms.size()) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "m_machina_forms size mismatch:"
        << "\n";

    oss << conmat::Indent(2) << "actual: "
        << conmat::Colorize(actual.m_machina_forms.size(), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_machina_forms.size(),
                            conmat::Color::Blue)
        << "\n";
  }

  // Check holding form pointers
  bool holding_form_match = (actual.m_holding_form == nullptr &&
                             m_expected.m_holding_form == nullptr) ||
                            (actual.m_holding_form != nullptr &&
                             m_expected.m_holding_form != nullptr);

  if (!holding_form_match) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "m_holding_form:"
        << "\n";

    if (actual.m_holding_form == nullptr) {
      oss << conmat::Indent(2)
          << "actual: " << conmat::Colorize("nullptr", conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize("non-nullptr", conmat::Color::Blue) << "\n";
    } else {
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize("non-nullptr", conmat::Color::Red) << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize("nullptr", conmat::Color::Blue) << "\n";
    }
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string CGrimoireMachinaEqualsMatcher::describe() const {
  return GetHeader() + m_mismatch_description;
}

} // namespace steamrot::tests
