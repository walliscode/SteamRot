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
  /// @brief Constructor for CUserInterfaceEqualsMatcher with entity index
  ///
  /// @param expected Expected CUserInterface object to compare against
  /// @param entity_index Index of the entity in the EntityMemoryPool
  /////////////////////////////////////////////////
  CUserInterfaceEqualsMatcher(const CUserInterface &expected, 
                              size_t entity_index);

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

/////////////////////////////////////////////////
/// @brief Helper function to create CUserInterfaceEqualsMatcher with entity index
///
/// @param expected Expected CUserInterface object
/// @param entity_index Index of the entity in the EntityMemoryPool
/// @return CUserInterfaceEqualsMatcher instance
/////////////////////////////////////////////////
inline CUserInterfaceEqualsMatcher
EqualsCUserInterface(const CUserInterface &expected, size_t entity_index) {
  return CUserInterfaceEqualsMatcher(expected, entity_index);
}

} // namespace steamrot::tests
