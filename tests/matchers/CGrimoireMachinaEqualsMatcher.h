/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for CGrimoireMachina comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CGrimoireMachina.h"
#include "ComponentMatcherBase.h"
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CGrimoireMachinaEqualsMatcher
/// @brief CGrimoireMachina specific matcher for detailed comparison
/////////////////////////////////////////////////
class CGrimoireMachinaEqualsMatcher
    : public ComponentMatcherBase<CGrimoireMachina> {
private:
  /////////////////////////////////////////////////
  /// @brief Get the component type name for display
  ///
  /// @return "CGrimoireMachina" string
  /////////////////////////////////////////////////
  std::string GetComponentName() const override;

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
