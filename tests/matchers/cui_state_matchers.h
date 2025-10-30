/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for CUIState comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CUIState.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class UIVisibilityState specific matcher
/////////////////////////////////////////////////
class UIVisibilityStateEqualsMatcher
    : public Catch::Matchers::MatcherBase<UIVisibilityState> {
private:
  const UIVisibilityState &m_expected;
  mutable std::string m_mismatch_description;

public:
  explicit UIVisibilityStateEqualsMatcher(const UIVisibilityState &expected)
      : m_expected(expected) {}

  bool match(const UIVisibilityState &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    if (actual.m_ui_indices_on != m_expected.m_ui_indices_on) {
      oss << "m_ui_indices_on differs; ";
    }

    if (actual.m_ui_indices_off != m_expected.m_ui_indices_off) {
      oss << "m_ui_indices_off differs; ";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    if (m_mismatch_description.empty()) {
      return "equals UIVisibilityState";
    }
    return "UIVisibilityState mismatch: " + m_mismatch_description;
  }
};

inline UIVisibilityStateEqualsMatcher
EqualsUIVisibilityState(const UIVisibilityState &expected) {
  return UIVisibilityStateEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @class CUIState specific matcher
/////////////////////////////////////////////////
class CUIStateEqualsMatcher : public Catch::Matchers::MatcherBase<CUIState> {
private:
  const CUIState &m_expected;
  mutable std::string m_mismatch_description;

public:
  explicit CUIStateEqualsMatcher(const CUIState &expected)
      : m_expected(expected) {}

  bool match(const CUIState &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    if (actual.m_active != m_expected.m_active) {
      oss << "m_active: actual=" << actual.m_active
          << ", expected=" << m_expected.m_active << "; ";
    }

    if (actual.m_state_to_ui_visibility.size() !=
        m_expected.m_state_to_ui_visibility.size()) {
      oss << "m_state_to_ui_visibility size differs; ";
    }

    if (actual.m_state_values != m_expected.m_state_values) {
      oss << "m_state_values differs; ";
    }

    if (actual.m_state_subscribers.size() != m_expected.m_state_subscribers.size()) {
      oss << "m_state_subscribers size differs; ";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    if (m_mismatch_description.empty()) {
      return "equals CUIState";
    }
    return "CUIState mismatch: " + m_mismatch_description;
  }
};

inline CUIStateEqualsMatcher EqualsCUIState(const CUIState &expected) {
  return CUIStateEqualsMatcher(expected);
}

} // namespace steamrot::tests
