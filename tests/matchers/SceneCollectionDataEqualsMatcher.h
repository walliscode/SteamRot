/////////////////////////////////////////////////
/// @file
/// @brief Declaration of SceneCollectionDataEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "SceneData.h"
#include "test_context.h"
#include "catch2/matchers/catch_matchers.hpp"
#include <optional>
#include <vector>

namespace steamrot::tests {

class SceneCollectionDataEqualsMatcher
    : public Catch::Matchers::MatcherBase<SceneCollectionData> {

private:
  /////////////////////////////////////////////////
  /// @brief reference to the expected SceneCollectionData vector
  /////////////////////////////////////////////////
  const SceneCollectionData &m_expected;

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
  /// @brief Constructor for SceneCollectionDataEqualsMatcher
  ///
  /// @param expected Reference to the expected SceneCollectionData vector
  /////////////////////////////////////////////////
  explicit SceneCollectionDataEqualsMatcher(
      const SceneCollectionData &expected);

  /////////////////////////////////////////////////
  /// @brief Constructor with TestContext
  ///
  /// @param expected Reference to the expected SceneCollectionData vector
  /// @param context Test context with metadata and tick information
  /////////////////////////////////////////////////
  SceneCollectionDataEqualsMatcher(const SceneCollectionData &expected,
                                   const TestContext &context);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual SceneCollectionData with expected
  ///
  /// @param actual Actual SceneCollectionData vector to compare
  /// @return whether the actual SceneCollectionData matches the expected
  /////////////////////////////////////////////////
  bool match(const SceneCollectionData &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create SceneCollectionDataEqualsMatcher
///
/// @param expected Expected SceneCollectionData vector
/// @return SceneCollectionDataEqualsMatcher instance
/////////////////////////////////////////////////
inline SceneCollectionDataEqualsMatcher
EqualsSceneCollection(const SceneCollectionData &expected) {
  return SceneCollectionDataEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @brief Helper function to create SceneCollectionDataEqualsMatcher with
/// context
///
/// @param expected Expected SceneCollectionData vector
/// @param context Test context with metadata and tick information
/// @return SceneCollectionDataEqualsMatcher instance
/////////////////////////////////////////////////
inline SceneCollectionDataEqualsMatcher
EqualsSceneCollection(const SceneCollectionData &expected,
                      const TestContext &context) {
  return SceneCollectionDataEqualsMatcher(expected, context);
}
} // namespace steamrot::tests
