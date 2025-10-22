/////////////////////////////////////////////////
/// @file
/// @brief Custom Catch2 matchers for Component comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Component.h"
#include "containers.h"
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

    // Check root element pointers
    bool root_elem_match =
        (actual.m_root_element == nullptr &&
         m_expected.m_root_element == nullptr) ||
        (actual.m_root_element != nullptr &&
         m_expected.m_root_element != nullptr &&
         actual.m_root_element.get() == m_expected.m_root_element.get());

    if (!root_elem_match) {
      oss << "m_root_element: pointers differ; ";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    std::ostringstream oss;
    oss << "equals CUserInterface(m_name='" << m_expected.m_name << "')";
    return oss.str();
  }

  std::string get_mismatch_description() const { return m_mismatch_description; }
};

inline CUserInterfaceEqualsMatcher
EqualsCUserInterface(const CUserInterface &expected) {
  return CUserInterfaceEqualsMatcher(expected);
}

} // namespace steamrot::tests
