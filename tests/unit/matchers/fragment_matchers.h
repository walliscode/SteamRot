/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for Fragment comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Fragment.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class Fragment specific matcher
/////////////////////////////////////////////////
class FragmentEqualsMatcher : public Catch::Matchers::MatcherBase<Fragment> {
private:
  const Fragment &m_expected;
  mutable std::string m_mismatch_description;

public:
  explicit FragmentEqualsMatcher(const Fragment &expected)
      : m_expected(expected) {}

  bool match(const Fragment &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    if (actual.m_name != m_expected.m_name) {
      oss << "m_name: actual='" << actual.m_name << "', expected='"
          << m_expected.m_name << "'; ";
    }

    if (actual.m_sockets != m_expected.m_sockets) {
      oss << "m_sockets differs; ";
    }

    // Compare transforms (matrix elements)
    const auto *this_matrix = actual.m_transform.getMatrix();
    const auto *other_matrix = m_expected.m_transform.getMatrix();
    bool transform_matches = true;
    for (size_t i = 0; i < 16; ++i) {
      if (this_matrix[i] != other_matrix[i]) {
        transform_matches = false;
        break;
      }
    }
    if (!transform_matches) {
      oss << "m_transform differs; ";
    }

    if (actual.m_overlays.size() != m_expected.m_overlays.size()) {
      oss << "m_overlays size differs; ";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    std::ostringstream oss;
    oss << "equals Fragment(m_name='" << m_expected.m_name << "')";
    return oss.str();
  }

  std::string get_mismatch_description() const { return m_mismatch_description; }
};

inline FragmentEqualsMatcher EqualsFragment(const Fragment &expected) {
  return FragmentEqualsMatcher(expected);
}

} // namespace steamrot::tests
