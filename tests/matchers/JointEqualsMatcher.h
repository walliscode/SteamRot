/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for Joint comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Joint.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class JointEqualsMatcher
/// @brief Joint specific matcher for detailed comparison
/////////////////////////////////////////////////
class JointEqualsMatcher : public Catch::Matchers::MatcherBase<Joint> {
private:
  const Joint &m_expected;
  mutable std::string m_mismatch_description;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for JointEqualsMatcher
  ///
  /// @param expected Expected Joint object to compare against
  /////////////////////////////////////////////////
  explicit JointEqualsMatcher(const Joint &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual Joint with expected
  ///
  /// @param actual Joint object to compare
  /// @return Whether the actual Joint matches the expected
  /////////////////////////////////////////////////
  bool match(const Joint &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create JointEqualsMatcher
///
/// @param expected Expected Joint object
/// @return JointEqualsMatcher instance
/////////////////////////////////////////////////
inline JointEqualsMatcher EqualsJoint(const Joint &expected) {
  return JointEqualsMatcher(expected);
}

} // namespace steamrot::tests
