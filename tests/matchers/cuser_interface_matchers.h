/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for CUserInterface comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CUserInterface.h"
#include "conmat.h"
#include "indentation_helpers.h"
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
      oss << formatting::IndentedLine(conmat::TestFailed() + "m_active", 1)
          << "\n";
      oss << formatting::IndentedKeyValue(
                 "actual", conmat::Colorize(actual.m_active, conmat::Color::Red),
                 2)
          << "\n";
      oss << formatting::IndentedKeyValue(
                 "expected",
                 conmat::Colorize(m_expected.m_active, conmat::Color::Blue), 2)
          << "\n";
    }

    if (actual.m_name != m_expected.m_name) {
      oss << formatting::IndentedLine(conmat::TestFailed() + "m_name", 1)
          << "\n";
      oss << formatting::IndentedKeyValue(
                 "actual", conmat::Colorize(actual.m_name, conmat::Color::Red), 2)
          << "\n";
      oss << formatting::IndentedKeyValue(
                 "expected",
                 conmat::Colorize(m_expected.m_name, conmat::Color::Blue), 2)
          << "\n";
    }

    if (actual.m_UI_visible != m_expected.m_UI_visible) {
      oss << formatting::IndentedLine(conmat::TestFailed() + "m_UI_visible", 1)
          << "\n";
      oss << formatting::IndentedKeyValue(
                 "actual",
                 conmat::Colorize(actual.m_UI_visible, conmat::Color::Red), 2)
          << "\n";
      oss << formatting::IndentedKeyValue(
                 "expected",
                 conmat::Colorize(m_expected.m_UI_visible, conmat::Color::Blue),
                 2)
          << "\n";
    }

    // Check root element pointers - both null or both non-null
    bool both_null = (actual.m_root_element == nullptr &&
                      m_expected.m_root_element == nullptr);
    bool both_non_null = (actual.m_root_element != nullptr &&
                          m_expected.m_root_element != nullptr);

    if (!both_null && !both_non_null) {
      oss << formatting::IndentedLine(conmat::TestFailed() + "m_root_element",
                                      1)
          << "\n";
      if (actual.m_root_element == nullptr) {
        oss << formatting::IndentedKeyValue(
                   "actual", conmat::Colorize("nullptr", conmat::Color::Red), 2)
            << "\n";
        oss << formatting::IndentedKeyValue(
                   "expected",
                   conmat::Colorize("non-nullptr", conmat::Color::Blue), 2)
            << "\n";
      } else {
        oss << formatting::IndentedKeyValue(
                   "actual",
                   conmat::Colorize("non-nullptr", conmat::Color::Red), 2)
            << "\n";
        oss << formatting::IndentedKeyValue(
                   "expected", conmat::Colorize("nullptr", conmat::Color::Blue),
                   2)
            << "\n";
      }
    } else if (both_non_null) {
      // Both are non-null, use UIElement matcher to compare them recursively
      UIElementEqualsMatcher element_matcher(*m_expected.m_root_element);
      if (!element_matcher.match(*actual.m_root_element)) {

        oss << formatting::IndentedLine(
                   conmat::TestFailed() + "m_root_element differences", 1)
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

  std::string describe() const override {

    // if mismatch description is empty then we can assume the test passed
    if (m_mismatch_description.empty()) {

      std::ostringstream oss;
      oss << conmat::Divider("-", 60) << "\n";
      oss << conmat::TestPassed() << "CUserInterface Match" << "\n";
      oss << conmat::Divider("-", 60) << "\n";
      return oss.str();
    } else {

      std::ostringstream oss;
      oss << conmat::Divider("-", 60) << "\n";
      oss << conmat::TestFailed() << "CUserInterface Match" << "\n";
      oss << m_mismatch_description;
      oss << conmat::Divider("-", 60) << "\n";

      return oss.str();
    }
  }
};

inline CUserInterfaceEqualsMatcher
EqualsCUserInterface(const CUserInterface &expected) {
  return CUserInterfaceEqualsMatcher(expected);
}

} // namespace steamrot::tests
