/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for CUserInterface comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "CUserInterface.h"
#include "ComponentMatcherBase.h"
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class CUserInterfaceEqualsMatcher
/// @brief CUserInterface specific matcher for detailed comparison
/////////////////////////////////////////////////
class CUserInterfaceEqualsMatcher
    : public ComponentMatcherBase<CUserInterface> {
private:
  /////////////////////////////////////////////////
  /// @brief Get the component type name for display
  ///
  /// @return "CUserInterface" string
  /////////////////////////////////////////////////
  std::string GetComponentName() const override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CUserInterfaceEqualsMatcher
  ///
  /// @param expected Expected CUserInterface object to compare against
  /////////////////////////////////////////////////
  explicit CUserInterfaceEqualsMatcher(const CUserInterface &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual CUserInterface with expected
  ///
  /// @param actual CUserInterface object to compare
  /// @return Whether the actual CUserInterface matches the expected
  /////////////////////////////////////////////////
  bool match(const CUserInterface &actual) const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create CUserInterfaceEqualsMatcher
///
/// @param expected Expected CUserInterface object
/// @return CUserInterfaceEqualsMatcher instance
/////////////////////////////////////////////////
inline CUserInterfaceEqualsMatcher
EqualsCUserInterface(const CUserInterface &expected) {
  return CUserInterfaceEqualsMatcher(expected);
}

} // namespace steamrot::tests
