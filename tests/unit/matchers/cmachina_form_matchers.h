/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for CMachinaForm comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CMachinaForm.h"
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
      oss << "m_active: actual=" << actual.m_active
          << ", expected=" << m_expected.m_active << "; ";
    }

    if (actual.m_fragments.size() != m_expected.m_fragments.size()) {
      oss << "m_fragments size: actual=" << actual.m_fragments.size()
          << ", expected=" << m_expected.m_fragments.size() << "; ";
    }

    if (actual.m_joints.size() != m_expected.m_joints.size()) {
      oss << "m_joints size: actual=" << actual.m_joints.size()
          << ", expected=" << m_expected.m_joints.size() << "; ";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    std::ostringstream oss;
    oss << "equals CMachinaForm";
    return oss.str();
  }

  std::string get_mismatch_description() const { return m_mismatch_description; }
};

inline CMachinaFormEqualsMatcher EqualsCMachinaForm(const CMachinaForm &expected) {
  return CMachinaFormEqualsMatcher(expected);
}

} // namespace steamrot::tests
