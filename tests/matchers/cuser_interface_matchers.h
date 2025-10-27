/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for CUserInterface comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CUserInterface.h"
#include "ui_element_matchers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CUserInterface specific matcher
/////////////////////////////////////////////////
class CUserInterfaceEqualsMatcher
    : public Catch::Matchers::MatcherBase<CUserInterface> {
private:
  const CUserInterface &m_expected;
  mutable std::string m_mismatch_description;

public:
  explicit CUserInterfaceEqualsMatcher(const CUserInterface &expected)
      : m_expected(expected) {}

  bool match(const CUserInterface &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    if (actual.m_active != m_expected.m_active) {
      oss << "m_active: actual=" << actual.m_active
          << ", expected=" << m_expected.m_active << "; ";
    }

    if (actual.m_name != m_expected.m_name) {
      oss << "m_name: actual='" << actual.m_name << "', expected='"
          << m_expected.m_name << "'; ";
    }

    if (actual.m_UI_visible != m_expected.m_UI_visible) {
      oss << "m_UI_visible: actual=" << actual.m_UI_visible
          << ", expected=" << m_expected.m_UI_visible << "; ";
    }

    // Check root element pointers - both null or both non-null
    bool both_null = (actual.m_root_element == nullptr &&
                      m_expected.m_root_element == nullptr);
    bool both_non_null = (actual.m_root_element != nullptr &&
                          m_expected.m_root_element != nullptr);

    if (!both_null && !both_non_null) {
      oss << "m_root_element: one is null, other is not; ";
    } else if (both_non_null) {
      // Both are non-null, use UIElement matcher to compare them recursively
      UIElementEqualsMatcher element_matcher(*m_expected.m_root_element);
      if (!element_matcher.match(*actual.m_root_element)) {
        oss << "m_root_element: " << element_matcher.get_mismatch_description();
      }
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    return "CUserInterface mismatch: " + m_mismatch_description;
  }

  std::string get_mismatch_description() const {
    return m_mismatch_description;
  }
};

inline CUserInterfaceEqualsMatcher
EqualsCUserInterface(const CUserInterface &expected) {
  return CUserInterfaceEqualsMatcher(expected);
}

} // namespace steamrot::tests
