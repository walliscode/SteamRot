/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for CUIState comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CUIState.h"
#include "ComponentMatcherBase.h"
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CUIStateEqualsMatcher
/// @brief CUIState specific matcher for detailed comparison
/////////////////////////////////////////////////
class CUIStateEqualsMatcher : public ComponentMatcherBase<CUIState> {
private:
  /////////////////////////////////////////////////
  /// @brief Get the component type name for display
  ///
  /// @return "CUIState" string
  /////////////////////////////////////////////////
  std::string GetComponentName() const override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CUIStateEqualsMatcher
  ///
  /// @param expected Expected CUIState object to compare against
  /////////////////////////////////////////////////
  explicit CUIStateEqualsMatcher(const CUIState &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual CUIState with expected
  ///
  /// @param actual CUIState object to compare
  /// @return Whether the actual CUIState matches the expected
  /////////////////////////////////////////////////
  bool match(const CUIState &actual) const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create CUIStateEqualsMatcher
///
/// @param expected Expected CUIState object
/// @return CUIStateEqualsMatcher instance
/////////////////////////////////////////////////
inline CUIStateEqualsMatcher EqualsCUIState(const CUIState &expected) {
  return CUIStateEqualsMatcher(expected);
}

} // namespace steamrot::tests
