/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for CGrimoireMachina comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CGrimoireMachina.h"
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
      oss << "m_active: actual=" << actual.m_active
          << ", expected=" << m_expected.m_active << "; ";
    }

    if (actual.m_all_fragments.size() != m_expected.m_all_fragments.size()) {
      oss << "m_all_fragments size: actual=" << actual.m_all_fragments.size()
          << ", expected=" << m_expected.m_all_fragments.size() << "; ";
    }

    if (actual.m_all_joints.size() != m_expected.m_all_joints.size()) {
      oss << "m_all_joints size: actual=" << actual.m_all_joints.size()
          << ", expected=" << m_expected.m_all_joints.size() << "; ";
    }

    if (actual.m_machina_forms.size() != m_expected.m_machina_forms.size()) {
      oss << "m_machina_forms size: actual=" << actual.m_machina_forms.size()
          << ", expected=" << m_expected.m_machina_forms.size() << "; ";
    }

    // Check holding form pointers
    bool holding_form_match =
        (actual.m_holding_form == nullptr &&
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
    if (m_mismatch_description.empty()) {
      return "equals CGrimoireMachina";
    }
    return "CGrimoireMachina mismatch: " + m_mismatch_description;
  }
};

inline CGrimoireMachinaEqualsMatcher
EqualsCGrimoireMachina(const CGrimoireMachina &expected) {
  return CGrimoireMachinaEqualsMatcher(expected);
}

} // namespace steamrot::tests
