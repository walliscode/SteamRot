/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for CMachinaForm comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CMachinaForm.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CMachinaForm specific matcher
/////////////////////////////////////////////////
class CMachinaFormEqualsMatcher
    : public Catch::Matchers::MatcherBase<CMachinaForm> {
private:
  const CMachinaForm &m_expected;
  mutable std::string m_mismatch_description;

public:
  explicit CMachinaFormEqualsMatcher(const CMachinaForm &expected)
      : m_expected(expected) {}

  bool match(const CMachinaForm &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    if (actual.m_active != m_expected.m_active) {
      oss << "\t" << conmat::TestFailed() << "m_active:" << "\n";

      oss << "\t\t" << "actual = "
          << conmat::Colorize(actual.m_active, conmat::Color::Red) << "\n";
      oss << "\t\t" << "expected = "
          << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
    }

    if (actual.m_fragments.size() != m_expected.m_fragments.size()) {
      oss << "\t" << conmat::TestFailed()
          << "m_fragments size mismatch:" << "\n";
      oss << "\t\t" << "actual size = "
          << conmat::Colorize(actual.m_fragments.size(), conmat::Color::Red)
          << "\n";
      oss << "\t\t" << "expected size = "
          << conmat::Colorize(m_expected.m_fragments.size(),
                              conmat::Color::Blue)
          << "\n";
    }

    if (actual.m_joints.size() != m_expected.m_joints.size()) {
      oss << "\t" << conmat::TestFailed() << "m_joints size mismatch:"
          << "\n";
      oss << "\t\t" << "actual size = "
          << conmat::Colorize(actual.m_joints.size(), conmat::Color::Red)
          << "\n";
      oss << "\t\t" << "expected size = "
          << conmat::Colorize(m_expected.m_joints.size(), conmat::Color::Blue)
          << "\n";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    // if mismatch description is empty then we can assume the test passed
    if (m_mismatch_description.empty()) {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestPassed() << "CMachinaForm Match" << "\n";
      oss << conmat::Divider("-", 40) << "\n";
      return oss.str();
    } else {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestFailed() << "CMachinaForm Match: " << "\n";
      oss << m_mismatch_description << "\n";
      oss << conmat::Divider("-", 40) << "\n";

      return oss.str();
    }
  }
};

inline CMachinaFormEqualsMatcher
EqualsCMachinaForm(const CMachinaForm &expected) {
  return CMachinaFormEqualsMatcher(expected);
}

} // namespace steamrot::tests
