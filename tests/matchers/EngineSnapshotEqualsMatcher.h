/////////////////////////////////////////////////
/// @file
/// @brief Declaration of EngineSnapshotEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineSnapshot.h"
#include "catch2/matchers/catch_matchers.hpp"
#include "test_context.h"
#include <optional>

namespace steamrot::tests {

class EngineSnapshotEqualsMatcher
    : public Catch::Matchers::MatcherBase<EngineSnapshot> {

private:
  /////////////////////////////////////////////////
  /// @brief reference to the expected EngineSnapshot
  /////////////////////////////////////////////////
  const EngineSnapshot &m_expected;

  /////////////////////////////////////////////////
  /// @brief Description of the mismatch found during comparison
  /////////////////////////////////////////////////
  mutable std::string m_mismatch_description;

  /////////////////////////////////////////////////
  /// @brief Optional test context for enriched error messages
  /////////////////////////////////////////////////
  std::optional<TestContext> m_context;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for EngineSnapshotEqualsMatcher
  ///
  /// @param expected Reference to the expected EngineSnapshot
  /////////////////////////////////////////////////
  explicit EngineSnapshotEqualsMatcher(const EngineSnapshot &expected);

  /////////////////////////////////////////////////
  /// @brief Constructor with TestContext
  ///
  /// @param expected Reference to the expected EngineSnapshot
  /// @param context Test context with metadata and tick information
  /////////////////////////////////////////////////
  EngineSnapshotEqualsMatcher(const EngineSnapshot &expected,
                              const TestContext &context);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual EngineSnapshot with expected
  ///
  /// @param actual Actual EngineSnapshot to compare
  /// @return whether the actual EngineSnapshot matches the expected
  /////////////////////////////////////////////////
  bool match(const EngineSnapshot &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create EngineSnapshotEqualsMatcher
///
/// @param expected Expected EngineSnapshot
/// @return EngineSnapshotEqualsMatcher instance
/////////////////////////////////////////////////
inline EngineSnapshotEqualsMatcher
EqualsEngineSnapshot(const EngineSnapshot &expected) {
  return EngineSnapshotEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @brief Helper function to create EngineSnapshotEqualsMatcher with context
///
/// @param expected Expected EngineSnapshot
/// @param context Test context with metadata and tick information
/// @return EngineSnapshotEqualsMatcher instance
/////////////////////////////////////////////////
inline EngineSnapshotEqualsMatcher
EqualsEngineSnapshot(const EngineSnapshot &expected,
                     const TestContext &context) {
  return EngineSnapshotEqualsMatcher(expected, context);
}

} // namespace steamrot::tests
