/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for CGrimoireMachina comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachinaEqualsMatcher.h"
#include "conmat.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::string CGrimoireMachinaEqualsMatcher::GetComponentName() const {
  return "CGrimoireMachina";
}

/////////////////////////////////////////////////
CGrimoireMachinaEqualsMatcher::CGrimoireMachinaEqualsMatcher(
    const CGrimoireMachina &expected)
    : ComponentMatcherBase<CGrimoireMachina>(expected) {}

/////////////////////////////////////////////////
bool CGrimoireMachinaEqualsMatcher::match(
    const CGrimoireMachina &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  if (actual.m_active != m_expected.m_active) {

    oss << "\t" << conmat::TestFailed() << "m_active:"
        << "\n";
    oss << "\t\t"
        << "actual = " << actual.m_active << "\n";
    oss << "\t\t"
        << "expected = " << m_expected.m_active << "\n";
  }

  if (actual.m_all_fragments.size() != m_expected.m_all_fragments.size()) {
    oss << "\t" << conmat::TestFailed() << "m_all_fragments size mismatch:"
        << "\n";
    oss << "\t\t"
        << "actual size = " << actual.m_all_fragments.size() << "\n";
    oss << "\t\t"
        << "expected size = " << m_expected.m_all_fragments.size() << "\n";
  }

  if (actual.m_all_joints.size() != m_expected.m_all_joints.size()) {
    oss << "\t" << conmat::TestFailed() << "m_all_joints size mismatch:"
        << "\n";
    oss << "\t\t"
        << "actual size = " << actual.m_all_joints.size() << "\n";
    oss << "\t\t"
        << "expected size = " << m_expected.m_all_joints.size() << "\n";
  }

  if (actual.m_machina_forms.size() != m_expected.m_machina_forms.size()) {
    oss << "\t" << conmat::TestFailed() << "m_machina_forms size mismatch:"
        << "\n";
    oss << "\t\t"
        << "actual size = " << actual.m_machina_forms.size() << "\n";
    oss << "\t\t"
        << "expected size = " << m_expected.m_machina_forms.size() << "\n";
  }

  // Check holding form pointers
  bool holding_form_match = (actual.m_holding_form == nullptr &&
                             m_expected.m_holding_form == nullptr) ||
                            (actual.m_holding_form != nullptr &&
                             m_expected.m_holding_form != nullptr);

  if (!holding_form_match) {
    oss << "m_holding_form: pointer nullness differs; ";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

} // namespace steamrot::tests
