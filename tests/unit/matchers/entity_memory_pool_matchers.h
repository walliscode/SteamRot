/////////////////////////////////////////////////
/// @file
/// @brief Custom Catch2 matchers for EntityMemoryPool comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "containers.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

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
  bool CompareComponentVector(const EntityMemoryPool &actual_pool,
                              const EntityMemoryPool &expected_pool,
                              const std::string &component_name,
                              std::ostringstream &oss) const {
    const auto &actual_vec =
        entity::memory::GetComponentVector<TComponent>(actual_pool);
    const auto &expected_vec =
        entity::memory::GetComponentVector<TComponent>(expected_pool);

    if (actual_vec.size() != expected_vec.size()) {
      oss << component_name << " vector size differs: actual="
          << actual_vec.size() << ", expected=" << expected_vec.size() << "; ";
      return false;
    }

    for (size_t i = 0; i < actual_vec.size(); ++i) {
      if (!(actual_vec[i] == expected_vec[i])) {
        oss << component_name << " at index " << i << " differs; ";
        return false;
      }
    }

    return true;
  }

  ////////////////////////////////////////////////////////////
  /// @brief Compare all component vectors in the tuple
  ///
  /// @param actual_pool The actual pool
  /// @param expected_pool The expected pool
  /// @param oss Output string stream for error messages
  /// @return true if all vectors match, false otherwise
  ////////////////////////////////////////////////////////////
  bool CompareAllComponentVectors(const EntityMemoryPool &actual_pool,
                                  const EntityMemoryPool &expected_pool,
                                  std::ostringstream &oss) const {
    bool all_match = true;

    // Compare CMeta
    all_match &= CompareComponentVector<CMeta>(actual_pool, expected_pool,
                                                "CMeta", oss);

    // Compare CUserInterface
    all_match &= CompareComponentVector<CUserInterface>(
        actual_pool, expected_pool, "CUserInterface", oss);

    // Compare CMachinaForm
    all_match &= CompareComponentVector<CMachinaForm>(
        actual_pool, expected_pool, "CMachinaForm", oss);

    // Compare CGrimoireMachina
    all_match &= CompareComponentVector<CGrimoireMachina>(
        actual_pool, expected_pool, "CGrimoireMachina", oss);

    // Compare CUIState
    all_match &= CompareComponentVector<CUIState>(actual_pool, expected_pool,
                                                   "CUIState", oss);

    return all_match;
  }

public:
  explicit EntityMemoryPoolEqualsMatcher(const EntityMemoryPool &expected)
      : m_expected(expected) {}

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
      oss << "Pool sizes differ: actual=" << actual_size
          << ", expected=" << expected_size << "; ";
      m_mismatch_description = oss.str();
      return false;
    }

    // Compare all component vectors
    bool all_match = CompareAllComponentVectors(actual, m_expected, oss);

    m_mismatch_description = oss.str();
    return all_match;
  }

  ////////////////////////////////////////////////////////////
  /// @brief Describe the matcher
  ///
  /// @return Description string
  ////////////////////////////////////////////////////////////
  std::string describe() const override {
    std::ostringstream oss;
    size_t expected_size = entity::memory::GetMemoryPoolSize(m_expected);
    oss << "equals EntityMemoryPool with size=" << expected_size;
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
/// @brief Helper function to create EntityMemoryPoolEqualsMatcher
///
/// @param expected The expected EntityMemoryPool
/// @return EntityMemoryPoolEqualsMatcher instance
/////////////////////////////////////////////////
inline EntityMemoryPoolEqualsMatcher
EqualsEntityMemoryPool(const EntityMemoryPool &expected) {
  return EntityMemoryPoolEqualsMatcher(expected);
}

} // namespace steamrot::tests
