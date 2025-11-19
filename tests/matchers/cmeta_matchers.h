/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for CMeta comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CMeta.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CMeta specific matcher for detailed comparison
/////////////////////////////////////////////////
class CMetaEqualsMatcher : public Catch::Matchers::MatcherBase<CMeta> {
private:
  const CMeta &m_expected;
  mutable std::string m_mismatch_description;

public:
  explicit CMetaEqualsMatcher(const CMeta &expected) : m_expected(expected) {}

  bool match(const CMeta &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    if (actual.m_active != m_expected.m_active) {
      oss << "\t" << conmat::TestFailed() << "m_active:" << "\n";

      oss << "\t\t" << "actual = "
          << conmat::Colorize(actual.m_active, conmat::Color::Red) << "\n";
      oss << "\t\t" << "expected = "
          << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
    }

    if (actual.m_entity_active != m_expected.m_entity_active) {
      oss << "\t" << conmat::TestFailed() << "m_entity_active:" << "\n";
      oss << "\t\t" << "actual = " << actual.m_entity_active << "\n";
      oss << "\t\t" << "expected = " << m_expected.m_entity_active << "\n";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    // if mismatch description is empty then we can assume the test passed
    if (m_mismatch_description.empty()) {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestPassed() << "CMeta Match" << "\n";
      oss << conmat::Divider("-", 40) << "\n";
      return oss.str();
    } else {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestFailed() << "CMeta Match: " << "\n";
      oss << m_mismatch_description << "\n";
      oss << conmat::Divider("-", 40) << "\n";

      return oss.str();
    }
  }
};

inline CMetaEqualsMatcher EqualsCMeta(const CMeta &expected) {
  return CMetaEqualsMatcher(expected);
}

} // namespace steamrot::tests
