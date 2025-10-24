/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for CMeta comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CMeta.h"
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
      oss << "m_active: actual=" << actual.m_active
          << ", expected=" << m_expected.m_active << "; ";
    }

    if (actual.m_entity_active != m_expected.m_entity_active) {
      oss << "m_entity_active: actual=" << actual.m_entity_active
          << ", expected=" << m_expected.m_entity_active << "; ";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    std::ostringstream oss;
    oss << "equals CMeta(m_active=" << m_expected.m_active
        << ", m_entity_active=" << m_expected.m_entity_active << ")";
    return oss.str();
  }

  std::string get_mismatch_description() const { return m_mismatch_description; }
};

inline CMetaEqualsMatcher EqualsCMeta(const CMeta &expected) {
  return CMetaEqualsMatcher(expected);
}

} // namespace steamrot::tests
