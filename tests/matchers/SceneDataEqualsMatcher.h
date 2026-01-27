/////////////////////////////////////////////////
/// @file
/// @brief Declaration of SceneDataEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "SceneData.h"
#include "catch2/matchers/catch_matchers.hpp"
namespace steamrot::tests {

class SceneDataEqualsMatcher : public Catch::Matchers::MatcherBase<SceneData> {

private:
  /////////////////////////////////////////////////
  /// @brief reference to the expected SceneData object
  /////////////////////////////////////////////////
  const SceneData &m_expected;

  /////////////////////////////////////////////////
  /// @brief Description of the mismatch found during comparison
  /////////////////////////////////////////////////
  mutable std::string m_mismatch_description;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for SceneDataEqualsMatcher
  ///
  /// @param expected Reference to the expected SceneData object
  /////////////////////////////////////////////////
  explicit SceneDataEqualsMatcher(const SceneData &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual SceneData with expected
  ///
  /// @param actual Actual SceneData object to compare
  /// @return whether the actual SceneData matches the expected
  /////////////////////////////////////////////////
  bool match(const SceneData &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create SceneDataEqualsMatcher
///
/// @param expected Expected SceneData object
/// @return SceneDataEqualsMatcher instance
/////////////////////////////////////////////////
inline SceneDataEqualsMatcher EqualsSceneData(const SceneData &expected) {
  return SceneDataEqualsMatcher(expected);
}
} // namespace steamrot::tests
