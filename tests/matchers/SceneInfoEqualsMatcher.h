/////////////////////////////////////////////////
/// @file
/// @brief Delaration of Catch2 matcher for SceneInfoEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneInfo.h"
#include "catch2/matchers/catch_matchers.hpp"

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class SceneInfoEqualsMatcher
/// @brief Specific matcher for SceneInfo comparison
///
/////////////////////////////////////////////////
class SceneInfoEqualsMatcher : public Catch::Matchers::MatcherBase<SceneInfo> {

private:
  const SceneInfo &m_expected;
  mutable std::string m_mismatch_description;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for SceneInfoEqualsMatcher
  ///
  /// @param expected Expected SceneInfo object to compare against
  /////////////////////////////////////////////////
  explicit SceneInfoEqualsMatcher(const SceneInfo &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual SceneInfo with expected
  ///
  /// @param actual SceneInfo object to compare
  /// @return whether the actual SceneInfo matches the expected
  /////////////////////////////////////////////////
  bool match(const SceneInfo &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create SceneInfoEqualsMatcher
///
/// @param expected Expected SceneInfo object
/// @return SceneInfoEqualsMatcher instance
/////////////////////////////////////////////////
inline SceneInfoEqualsMatcher EqualsSceneInfo(const SceneInfo &expected) {
  return SceneInfoEqualsMatcher(expected);
}
} // namespace steamrot::tests
