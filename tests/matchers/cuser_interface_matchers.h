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

      oss << "\t" << conmat::TestFailed() << "m_active:" << "\n";
      oss << "\t\t" << "actual = "
          << conmat::Colorize(actual.m_active, conmat::Color::Red) << "\n";
      oss << "\t\t" << "expected = "
          << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
    }

    if (actual.m_name != m_expected.m_name) {
      oss << "\t" << conmat::TestFailed() << "m_name:" << "\n";
      oss << "\t\t"
          << "actual = " << conmat::Colorize(actual.m_name, conmat::Color::Red)
          << "\n";
      oss << "\t\t" << "expected = "
          << conmat::Colorize(m_expected.m_name, conmat::Color::Blue) << "\n";
    }

    if (actual.m_UI_visible != m_expected.m_UI_visible) {
      oss << "\t" << conmat::TestFailed() << "m_UI_visible:" << "\n";
      oss << "\t\t"
          << "actual = "
          << conmat::Colorize(actual.m_UI_visible, conmat::Color::Red) << "\n";
      oss << "\t\t"
          << "expected = "
          << conmat::Colorize(m_expected.m_UI_visible, conmat::Color::Blue)
          << "\n";
    }

    // Check root element pointers - both null or both non-null
    bool both_null = (actual.m_root_element == nullptr &&
                      m_expected.m_root_element == nullptr);
    bool both_non_null = (actual.m_root_element != nullptr &&
                          m_expected.m_root_element != nullptr);

    if (!both_null && !both_non_null) {
      oss << "\t" << conmat::TestFailed() << "m_root_element:" << "\n";
      if (actual.m_root_element == nullptr) {
        oss << "\t\t"
            << "actual = " << conmat::Colorize("nullptr", conmat::Color::Red)
            << "\n";
        oss << "\t\t"
            << "expected = "
            << conmat::Colorize("non-nullptr", conmat::Color::Blue) << "\n";
      } else {
        oss << "\t\t"
            << "actual = "
            << conmat::Colorize("non-nullptr", conmat::Color::Red) << "\n";
        oss << "\t\t"
            << "expected = " << conmat::Colorize("nullptr", conmat::Color::Blue)
            << "\n";
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

  std::string describe() const override {

    // if mismatch description is empty then we can assume the test passed
    if (m_mismatch_description.empty()) {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestPassed() << "CUserInterFace Match" << "\n";
      oss << conmat::Divider("-", 40) << "\n";
      return oss.str();
    } else {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestFailed() << "CUserInterFace Match: " << "\n";
      oss << m_mismatch_description << "\n";
      oss << conmat::Divider("-", 40) << "\n";

      return oss.str();
    }
  }
};

inline CUserInterfaceEqualsMatcher
EqualsCUserInterface(const CUserInterface &expected) {
  return CUserInterfaceEqualsMatcher(expected);
}

} // namespace steamrot::tests
