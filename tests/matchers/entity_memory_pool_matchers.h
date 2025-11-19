/////////////////////////////////////////////////
/// @file
/// @brief Custom Catch2 matchers for EntityMemoryPool comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "cgrimoire_machina_matchers.h"
#include "cmachina_form_matchers.h"
#include "cmeta_matchers.h"
#include "conmat.h"
#include "containers.h"
#include "cui_state_matchers.h"
#include "cuser_interface_matchers.h"
#include "entity_memory.h"
#include "test_context.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class EntityMemoryPoolEqualsMatcher
/// @brief Custom Catch2 matcher for comparing EntityMemoryPool objects
///
/// Provides detailed output when EntityMemoryPools don't match,
/// including early assertions for pool sizes
/////////////////////////////////////////////////
class EntityMemoryPoolEqualsMatcher
    : public Catch::Matchers::MatcherBase<EntityMemoryPool> {
private:
  const EntityMemoryPool &m_expected;
  mutable std::string m_mismatch_description;
  mutable bool do_components_match{true};
  std::string m_test_metadata;          // Legacy support
  std::optional<TestContext> m_context; // New context-based approach
  ////////////////////////////////////////////////////////////
  /// @brief Helper to compare component vectors
  ///
  /// @tparam TComponent The component type
  /// @param actual_pool The actual pool
  /// @param expected_pool The expected pool
  /// @param component_name Name of the component for error messages
  /// @return true if vectors match, false otherwise
  ////////////////////////////////////////////////////////////
  template <typename TComponent>
  void CompareComponentVector(const std::vector<TComponent> &actual_vec,
                              const std::vector<TComponent> &expected_pool,
                              std::ostringstream &oss) const {

    FAIL("Component type does not have a matcher set up");
  }

  template <>
  void CompareComponentVector(const std::vector<CMeta> &actual_vec,
                              const std::vector<CMeta> &expected_vec,
                              std::ostringstream &oss) const {

    for (size_t i = 0; i < expected_vec.size(); ++i) {
      CMetaEqualsMatcher matcher(expected_vec[i]);
      if (!matcher.match(actual_vec[i])) {
        // pass through the mismatch description from CMetaEqualsMatcher
        oss << matcher.describe() << "\n";

        // switch flag to false
        do_components_match = false;
      }
    }
  }

  template <>
  void CompareComponentVector(const std::vector<CUserInterface> &actual_vec,
                              const std::vector<CUserInterface> &expected_pool,
                              std::ostringstream &oss) const {

    for (size_t i = 0; i < expected_pool.size(); ++i) {
      CUserInterfaceEqualsMatcher matcher(expected_pool[i]);
      if (!matcher.match(actual_vec[i])) {
        // pass through the mismatch description from
        // CUserInterfaceEqualsMatcher
        oss << matcher.describe() << "\n";
        // switch flag to false
        do_components_match = false;
      }
    }
  }

  template <>
  void CompareComponentVector(const std::vector<CMachinaForm> &actual_vec,
                              const std::vector<CMachinaForm> &expected_pool,
                              std::ostringstream &oss) const {
    for (size_t i = 0; i < expected_pool.size(); ++i) {
      CMachinaFormEqualsMatcher matcher(expected_pool[i]);
      if (!matcher.match(actual_vec[i])) {

        // pass through the mismatch description from CMachinaFormEqualsMatcher
        oss << matcher.describe() << "\n";
        // switch flag to false
        do_components_match = false;
      }
    }
  }

  template <>
  void
  CompareComponentVector(const std::vector<CGrimoireMachina> &actual_vec,
                         const std::vector<CGrimoireMachina> &expected_pool,
                         std::ostringstream &oss) const {
    for (size_t i = 0; i < expected_pool.size(); ++i) {
      CGrimoireMachinaEqualsMatcher matcher(expected_pool[i]);
      if (!matcher.match(actual_vec[i])) {
        // pass through the mismatch description from
        // CGrimoireMachinaEqualsMatcher
        oss << matcher.describe() << "\n";
        // switch flag to false
        do_components_match = false;
      }
    }
  }
  template <>
  void CompareComponentVector(const std::vector<CUIState> &actual_vec,
                              const std::vector<CUIState> &expected_pool,
                              std::ostringstream &oss) const {

    for (size_t i = 0; i < expected_pool.size(); ++i) {
      CUIStateEqualsMatcher matcher(expected_pool[i]);
      if (!matcher.match(actual_vec[i])) {

        // pass through the mismatch description from CUIStateEqualsMatcher
        oss << matcher.describe() << "\n";

        // switch flag to false
        do_components_match = false;
      }
    }
  }

  template <std::size_t... Is>
  void CompareAllComponentVectorsImpl(const EntityMemoryPool &actual_pool,
                                      const EntityMemoryPool &expected_pool,
                                      std::ostringstream &oss,
                                      std::index_sequence<Is...>) const {

    // Expand the parameter pack to compare each component vector
    (CompareComponentVector<std::tuple_element_t<Is, ComponentRegister>>(
         std::get<Is>(actual_pool), std::get<Is>(expected_pool), oss),
     ...);
  }

  ////////////////////////////////////////////////////////////
  /// @brief Compare all component vectors in the tuple
  ///
  /// @param actual_pool The actual pool
  /// @param expected_pool The expected pool
  /// @param oss Output string stream for error messages
  /// @return true if all vectors match, false otherwise
  ////////////////////////////////////////////////////////////
  void CompareAllComponentVectors(const EntityMemoryPool &actual_pool,
                                  const EntityMemoryPool &expected_pool,
                                  std::ostringstream &oss) const {

    // Generate an index sequence for the number of components
    CompareAllComponentVectorsImpl(
        actual_pool, expected_pool, oss,
        std::make_index_sequence<kComponentRegisterSize>{});
  }

public:
  explicit EntityMemoryPoolEqualsMatcher(const EntityMemoryPool &expected)
      : m_expected(expected), m_test_metadata(""), m_context(std::nullopt) {}

  ////////////////////////////////////////////////////////////
  /// @brief Constructor with test metadata (legacy)
  ///
  /// @param expected The expected EntityMemoryPool
  /// @param test_metadata Optional test metadata (e.g., test name)
  ////////////////////////////////////////////////////////////
  EntityMemoryPoolEqualsMatcher(const EntityMemoryPool &expected,
                                const std::string &test_metadata)
      : m_expected(expected), m_test_metadata(test_metadata),
        m_context(std::nullopt) {}

  ////////////////////////////////////////////////////////////
  /// @brief Constructor with TestContext
  ///
  /// @param expected The expected EntityMemoryPool
  /// @param context Test context with metadata and tick information
  ////////////////////////////////////////////////////////////
  EntityMemoryPoolEqualsMatcher(const EntityMemoryPool &expected,
                                const TestContext &context)
      : m_expected(expected), m_test_metadata(""), m_context(context) {}

  ////////////////////////////////////////////////////////////
  /// @brief Check if the EntityMemoryPools match
  ///
  /// @param actual The actual EntityMemoryPool to compare
  /// @return true if pools match, false otherwise
  ////////////////////////////////////////////////////////////
  bool match(const EntityMemoryPool &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    // Early assertion: check pool sizes first
    size_t actual_size = entity::memory::GetMemoryPoolSize(actual);
    size_t expected_size = entity::memory::GetMemoryPoolSize(m_expected);

    if (actual_size != expected_size) {

      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestFailed() << "EntityMemoryPool Size Mismatch:" << "\n";
      oss << "\t"
          << "actual size = "
          << conmat::Colorize(actual_size, conmat::Color::Red) << "\n";
      oss << "\t"
          << "expected size = "
          << conmat::Colorize(expected_size, conmat::Color::Blue) << "\n";
      oss << conmat::Divider("-", 40) << "\n";

      m_mismatch_description = oss.str();
      return false;
    }

    // Compare each component vector in the pools
    CompareAllComponentVectors(actual, m_expected, oss);
    if (!do_components_match) {

      m_mismatch_description = oss.str();
      return false;
    }
    return true;
  }

  ////////////////////////////////////////////////////////////
  /// @brief Describe the matcher with formatted, hierarchical output
  ///
  /// @return Description string with visual formatting (no ANSI codes)
  ////////////////////////////////////////////////////////////
  std::string describe() const override {

    if (m_mismatch_description.empty()) {

      std::ostringstream oss;
      oss << conmat::Divider("=", 40) << "\n";
      oss << conmat::Colorize("[PASSED] ", conmat::Color::Green)
          << "EntityMemoryPool Match" << "\n";
      oss << conmat::Divider("=", 40) << "\n";
      return oss.str();
    } else {

      std::ostringstream oss;
      oss << conmat::Divider("=", 40) << "\n";
      oss << conmat::Colorize("[FAILED] ", conmat::Color::Red)
          << "EntityMemoryPool Match " << "\n";
      oss << m_mismatch_description << "\n";
      oss << conmat::Divider("=", 40) << "\n";

      return oss.str();
    }
  }
};

/////////////////////////////////////////////////
/// @brief Helper function to create EntityMemoryPoolEqualsMatcher
///
/// @param expected The expected EntityMemoryPool
/// @return EntityMemoryPoolEqualsMatcher instance
/////////////////////////////////////////////////
inline EntityMemoryPoolEqualsMatcher
EqualsEntityMemoryPool(const EntityMemoryPool &expected) {
  return EntityMemoryPoolEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @brief Helper function to create EntityMemoryPoolEqualsMatcher with metadata
///
/// @param expected The expected EntityMemoryPool
/// @param test_metadata Test metadata to include in failure messages
/// @return EntityMemoryPoolEqualsMatcher instance
/////////////////////////////////////////////////
inline EntityMemoryPoolEqualsMatcher
EqualsEntityMemoryPool(const EntityMemoryPool &expected,
                       const std::string &test_metadata) {
  return EntityMemoryPoolEqualsMatcher(expected, test_metadata);
}

/////////////////////////////////////////////////
/// @brief Helper function to create EntityMemoryPoolEqualsMatcher with context
///
/// @param expected The expected EntityMemoryPool
/// @param context Test context with metadata and tick information
/// @return EntityMemoryPoolEqualsMatcher instance
/////////////////////////////////////////////////
inline EntityMemoryPoolEqualsMatcher
EqualsEntityMemoryPool(const EntityMemoryPool &expected,
                       const TestContext &context) {
  return EntityMemoryPoolEqualsMatcher(expected, context);
}

} // namespace steamrot::tests
