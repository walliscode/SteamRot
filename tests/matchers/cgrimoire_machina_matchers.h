/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for CGrimoireMachina comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CGrimoireMachina.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CGrimoireMachina specific matcher
/////////////////////////////////////////////////
class CGrimoireMachinaEqualsMatcher
    : public Catch::Matchers::MatcherBase<CGrimoireMachina> {
private:
  const CGrimoireMachina &m_expected;
  mutable std::string m_mismatch_description;

public:
  explicit CGrimoireMachinaEqualsMatcher(const CGrimoireMachina &expected)
      : m_expected(expected) {}

  bool match(const CGrimoireMachina &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    if (actual.m_active != m_expected.m_active) {

      oss << "\t" << conmat::TestFailed() << "m_active:" << "\n";
      oss << "\t\t" << "actual = " << actual.m_active << "\n";
      oss << "\t\t" << "expected = " << m_expected.m_active << "\n";
    }

    if (actual.m_all_fragments.size() != m_expected.m_all_fragments.size()) {
      oss << "\t" << conmat::TestFailed() << "m_all_fragments size mismatch:"
          << "\n";
      oss << "\t\t" << "actual size = " << actual.m_all_fragments.size()
          << "\n";
      oss << "\t\t" << "expected size = " << m_expected.m_all_fragments.size()
          << "\n";
    }

    if (actual.m_all_joints.size() != m_expected.m_all_joints.size()) {
      oss << "\t" << conmat::TestFailed() << "m_all_joints size mismatch:"
          << "\n";
      oss << "\t\t" << "actual size = " << actual.m_all_joints.size() << "\n";
      oss << "\t\t" << "expected size = " << m_expected.m_all_joints.size()
          << "\n";
    }

    if (actual.m_machina_forms.size() != m_expected.m_machina_forms.size()) {
      oss << "\t" << conmat::TestFailed() << "m_machina_forms size mismatch:"
          << "\n";
      oss << "\t\t" << "actual size = " << actual.m_machina_forms.size()
          << "\n";
      oss << "\t\t" << "expected size = " << m_expected.m_machina_forms.size()
          << "\n";
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

  std::string describe() const override {

    // if mismatch description is empty then we can assume the test passed
    if (m_mismatch_description.empty()) {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestPassed() << "CGrimoireMachina Match" << "\n";
      oss << conmat::Divider("-", 40) << "\n";
      return oss.str();
    } else {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestFailed() << "CGrimoireMachina Match: " << "\n";
      oss << m_mismatch_description << "\n";
      oss << conmat::Divider("-", 40) << "\n";

      return oss.str();
    }
  }
};

inline CGrimoireMachinaEqualsMatcher
EqualsCGrimoireMachina(const CGrimoireMachina &expected) {
  return CGrimoireMachinaEqualsMatcher(expected);
}

} // namespace steamrot::tests
