/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for CGrimoireMachina comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CGrimoireMachina.h"
#include "conmat.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CGrimoireMachinaEqualsMatcher
/// @brief Matcher for detailed CGrimoireMachina comparison
/////////////////////////////////////////////////
class CGrimoireMachinaEqualsMatcher
    : public Catch::Matchers::MatcherBase<CGrimoireMachina> {
private:
  /////////////////////////////////////////////////
  /// @brief Expected CGrimoireMachina object to compare against
  /////////////////////////////////////////////////
  const CGrimoireMachina &m_expected;

  /////////////////////////////////////////////////
  /// @brief Store mismatch description for reporting
  ///
  /// @note mutable to allow modification in const match method
  /////////////////////////////////////////////////
  mutable std::string m_mismatch_description;

  /////////////////////////////////////////////////
  /// @brief Get formatted header for the matcher output
  ///
  /// @return Formatted header string
  /////////////////////////////////////////////////
  std::string GetHeader() const;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CGrimoireMachinaEqualsMatcher
  ///
  /// @param expected Expected CGrimoireMachina object to compare against
  /////////////////////////////////////////////////
  explicit CGrimoireMachinaEqualsMatcher(const CGrimoireMachina &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual CGrimoireMachina with expected
  ///
  /// @param actual CGrimoireMachina object to compare
  /// @return Whether the actual CGrimoireMachina matches the expected
  /////////////////////////////////////////////////
  bool match(const CGrimoireMachina &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe the matcher result
  ///
  /// @return Description string showing match result and any mismatches
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create CGrimoireMachinaEqualsMatcher
///
/// @param expected Expected CGrimoireMachina object
/// @return CGrimoireMachinaEqualsMatcher instance
/////////////////////////////////////////////////
inline CGrimoireMachinaEqualsMatcher
EqualsCGrimoireMachina(const CGrimoireMachina &expected) {
  return CGrimoireMachinaEqualsMatcher(expected);
}

} // namespace steamrot::tests
