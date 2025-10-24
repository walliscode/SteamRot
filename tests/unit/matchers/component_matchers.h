/////////////////////////////////////////////////
/// @file
/// @brief Generic Catch2 matcher for Component comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Component.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class ComponentEqualsMatcher
/// @brief Custom Catch2 matcher for comparing Component objects
///
/// Provides detailed output when Components don't match
/////////////////////////////////////////////////
template <typename TComponent>
class ComponentEqualsMatcher : public Catch::Matchers::MatcherBase<TComponent> {
  static_assert(std::is_base_of<Component, TComponent>::value,
                "TComponent must derive from Component");

private:
  const TComponent &m_expected;
  mutable std::string m_mismatch_description;

public:
  explicit ComponentEqualsMatcher(const TComponent &expected)
      : m_expected(expected) {}

  ////////////////////////////////////////////////////////////
  /// @brief Check if the component matches
  ///
  /// @param actual The actual component to compare
  /// @return true if components match, false otherwise
  ////////////////////////////////////////////////////////////
  bool match(const TComponent &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    // Compare base Component fields
    if (actual.m_active != m_expected.m_active) {
      oss << "m_active differs: actual=" << actual.m_active
          << ", expected=" << m_expected.m_active << "; ";
    }

    // Use equality operator for full comparison
    bool are_equal = (actual == m_expected);

    if (!are_equal && m_mismatch_description.empty()) {
      oss << "Components differ in derived class fields";
    }

    m_mismatch_description = oss.str();
    return are_equal;
  }

  ////////////////////////////////////////////////////////////
  /// @brief Describe the matcher
  ///
  /// @return Description string
  ////////////////////////////////////////////////////////////
  std::string describe() const override {
    std::ostringstream oss;
    oss << "equals Component with m_active=" << m_expected.m_active;
    return oss.str();
  }

  ////////////////////////////////////////////////////////////
  /// @brief Get the mismatch description
  ///
  /// @return Mismatch description string
  ////////////////////////////////////////////////////////////
  std::string get_mismatch_description() const { return m_mismatch_description; }
};

/////////////////////////////////////////////////
/// @brief Helper function to create ComponentEqualsMatcher
///
/// @param expected The expected component
/// @return ComponentEqualsMatcher instance
/////////////////////////////////////////////////
template <typename TComponent>
ComponentEqualsMatcher<TComponent> EqualsComponent(const TComponent &expected) {
  return ComponentEqualsMatcher<TComponent>(expected);
}

} // namespace steamrot::tests
