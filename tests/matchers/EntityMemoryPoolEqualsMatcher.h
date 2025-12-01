/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for EntityMemoryPool comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "containers.h"
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
  mutable bool do_components_match{true};
  std::string m_test_metadata;          // Legacy support
  std::optional<TestContext> m_context; // New context-based approach

  /////////////////////////////////////////////////
  /// @brief Helper to compare component vectors
  ///
  /// @tparam TComponent Component type
  /// @param actual_vec Actual component vector
  /// @param expected_vec Expected component vector
  /// @param oss Output stream for mismatch description
  /////////////////////////////////////////////////
  template <typename TComponent>
  void CompareComponentVector(const std::vector<TComponent> &actual_vec,
                              const std::vector<TComponent> &expected_vec,
                              std::ostringstream &oss) const;

  /////////////////////////////////////////////////
  /// @brief Compare all component vectors in the tuple
  ///
  /// @param actual_pool Actual pool
  /// @param expected_pool Expected pool
  /// @param oss Output string stream for error messages
  /////////////////////////////////////////////////
  void CompareAllComponentVectors(const EntityMemoryPool &actual_pool,
                                  const EntityMemoryPool &expected_pool,
                                  std::ostringstream &oss) const;

  /////////////////////////////////////////////////
  /// @brief Implementation helper for comparing all component vectors
  ///
  /// @param actual_pool Actual pool
  /// @param expected_pool Expected pool
  /// @param oss Output string stream for error messages
  /// @param index_seq Index sequence for tuple expansion
  /////////////////////////////////////////////////
  template <std::size_t... Is>
  void CompareAllComponentVectorsImpl(const EntityMemoryPool &actual_pool,
                                      const EntityMemoryPool &expected_pool,
                                      std::ostringstream &oss,
                                      std::index_sequence<Is...>) const;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for EntityMemoryPoolEqualsMatcher
  ///
  /// @param expected Expected EntityMemoryPool object to compare against
  /////////////////////////////////////////////////
  explicit EntityMemoryPoolEqualsMatcher(const EntityMemoryPool &expected);

  /////////////////////////////////////////////////
  /// @brief Constructor with test metadata (legacy)
  ///
  /// @param expected Expected EntityMemoryPool object to compare against
  /// @param test_metadata Optional test metadata (e.g., test name)
  /////////////////////////////////////////////////
  EntityMemoryPoolEqualsMatcher(const EntityMemoryPool &expected,
                                const std::string &test_metadata);

  /////////////////////////////////////////////////
  /// @brief Constructor with TestContext
  ///
  /// @param expected Expected EntityMemoryPool object to compare against
  /// @param context Test context with metadata and tick information
  /////////////////////////////////////////////////
  EntityMemoryPoolEqualsMatcher(const EntityMemoryPool &expected,
                                const TestContext &context);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual EntityMemoryPool with expected
  ///
  /// @param actual EntityMemoryPool object to compare
  /// @return Whether the actual EntityMemoryPool matches the expected
  /////////////////////////////////////////////////
  bool match(const EntityMemoryPool &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create EntityMemoryPoolEqualsMatcher
///
/// @param expected Expected EntityMemoryPool object
/// @return EntityMemoryPoolEqualsMatcher instance
/////////////////////////////////////////////////
inline EntityMemoryPoolEqualsMatcher
EqualsEntityMemoryPool(const EntityMemoryPool &expected) {
  return EntityMemoryPoolEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @brief Helper function to create EntityMemoryPoolEqualsMatcher with metadata
///
/// @param expected Expected EntityMemoryPool object
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
/// @param expected Expected EntityMemoryPool object
/// @param context Test context with metadata and tick information
/// @return EntityMemoryPoolEqualsMatcher instance
/////////////////////////////////////////////////
inline EntityMemoryPoolEqualsMatcher
EqualsEntityMemoryPool(const EntityMemoryPool &expected,
                       const TestContext &context) {
  return EntityMemoryPoolEqualsMatcher(expected, context);
}

} // namespace steamrot::tests
